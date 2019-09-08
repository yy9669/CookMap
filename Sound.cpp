#include "Sound.hpp"
#include "load_wav.hpp"
#include "load_opus.hpp"

#include <SDL.h>

#include <list>
#include <cassert>
#include <exception>
#include <iostream>
#include <algorithm>

//local (to this file) data used by the audio system:
namespace {

	//handy constants:
	constexpr uint32_t const AUDIO_RATE = 48000; //sampling rate
	constexpr uint32_t const MIX_SAMPLES = 1024; //number of samples to mix per call of mix_audio callback; n.b. SDL requires this to be a power of two

	//The audio device:
	SDL_AudioDeviceID device = 0;

	//list of all currently playing samples:
	std::list< std::shared_ptr< Sound::PlayingSample > > playing_samples;

}

//public-facing data:

//global volume control:
Sound::Ramp< float > Sound::volume = Sound::Ramp< float >(1.0f);

//This audio-mixing callback is defined below:
void mix_audio(void *, Uint8 *buffer_, int len);

//------------------------ public-facing --------------------------------

Sound::Sample::Sample(std::string const &filename) {
	if (filename.size() >= 4 && filename.substr(filename.size()-4) == ".png") {
		load_wav(filename, &data);
	} else if (filename.size() >= 5 && filename.substr(filename.size()-5) == ".opus") {
		load_opus(filename, &data);
	} else {
		throw std::runtime_error("Sample '" + filename + "' doesn't end in either \".png\" or \".opus\" -- unsure how to load.");
	}
}

Sound::Sample::Sample(std::vector< float > const &data_) : data(data_) {
}

void Sound::PlayingSample::stop(float ramp) {
	lock();
	if (!stopped) {
		stopped = true;
		volume.target = 0.0f;
		volume.ramp = ramp;
	} else {
		volume.ramp = std::min(volume.ramp, ramp);
	}
	unlock();
}


void Sound::init() {
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		std::cerr << "Failed to initialize SDL audio subsytem:\n" << SDL_GetError() << std::endl;
		std::cerr << "  (Will continue without audio.)\n" << std::endl;
		return;
	}

	//Based on the example on https://wiki.libsdl.org/SDL_OpenAudioDevice
	SDL_AudioSpec want, have;
	SDL_zero(want);
	want.freq = AUDIO_RATE;
	want.format = AUDIO_F32SYS;
	want.channels = 2;
	want.samples = MIX_SAMPLES;
	want.callback = mix_audio;

	device = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
	if (device == 0) {
		std::cerr << "Failed to open audio device:\n" << SDL_GetError() << std::endl;
		std::cerr << "  (Will continue without audio.)\n" << std::endl;
	} else {
		//start audio playback:
		SDL_PauseAudioDevice(device, 0);
		std::cout << "Audio output initialized." << std::endl;
	}
}


void Sound::shutdown() {
	if (device != 0) {
		//stop audio playback:
		SDL_PauseAudioDevice(device, 1);
		SDL_CloseAudioDevice(device);
		device = 0;
	}
}


void Sound::lock() {
	if (device) SDL_LockAudioDevice(device);
}

void Sound::unlock() {
	if (device) SDL_UnlockAudioDevice(device);
}

std::shared_ptr< Sound::PlayingSample > Sound::play(Sample const &sample, float pan, float volume) {
	std::shared_ptr< Sound::PlayingSample > playing_sample = std::make_shared< Sound::PlayingSample >(sample, pan, volume);
	lock();
	playing_samples.emplace_back(playing_sample);
	unlock();
	return playing_sample;
}


void Sound::stop_all_samples() {
	lock();
	for (auto &s : playing_samples) {
		s->stop();
	}
	unlock();
}

void Sound::set_volume(float new_volume, float ramp) {
	lock();
	volume.set(new_volume, ramp);
	unlock();
}


//------------------------ internals --------------------------------


//helper: equal-power panning
inline void compute_pan_weights(float pan, float *left, float *right) {
	//clamp pan to -1 to 1 range:
	pan = std::max(-1.0f, std::min(1.0f, pan));

	//want left^2 + right^2 = 1.0, so use angles:
	float ang = 0.5f * 3.1415926f * (0.5f * (pan + 1.0f));
	*left = std::cos(ang);
	*right = std::sin(ang);
}

//helper: ramp update for single values:
constexpr float const RAMP_STEP = float(MIX_SAMPLES) / float(AUDIO_RATE);
void step_value_ramp(Sound::Ramp< float > &ramp) {
	if (ramp.ramp < RAMP_STEP) {
		ramp.value = ramp.target;
		ramp.ramp = 0.0f;
	} else {
		ramp.value += (RAMP_STEP / ramp.ramp) * (ramp.target - ramp.value);
		ramp.ramp -= RAMP_STEP;
	}
}

//The audio callback -- invoked by SDL when it needs more sound to play:
void mix_audio(void *, Uint8 *buffer_, int len) {
	assert(buffer_); //should always have some audio buffer

	struct LR {
		float l;
		float r;
	};
	static_assert(sizeof(LR) == 8, "Sample is packed");
	assert(len == MIX_SAMPLES * sizeof(LR)); //should always have the expected number of samples
	LR *buffer = reinterpret_cast< LR * >(buffer_);

	//zero the output buffer:
	for (uint32_t s = 0; s < MIX_SAMPLES; ++s) {
		buffer[s].l = 0.0f;
		buffer[s].r = 0.0f;
	}

	//update global values:
	float start_volume = Sound::volume.value;
	step_value_ramp(Sound::volume);
	float end_volume = Sound::volume.value;

	//add audio from each playing sample into the buffer:
	for (auto si = playing_samples.begin(); si != playing_samples.end(); /* later */) {
		Sound::PlayingSample &playing_sample = **si; //much more convenient than writing ** everywhere.

		//Figure out sample panning/volume at start...
		LR start_pan;
		compute_pan_weights(playing_sample.pan.value, &start_pan.l, &start_pan.r);
		start_pan.l *= start_volume * playing_sample.volume.value;
		start_pan.r *= start_volume * playing_sample.volume.value;

		step_value_ramp(playing_sample.pan);
		step_value_ramp(playing_sample.volume);

		//..and end of the mix period:
		LR end_pan;
		compute_pan_weights(playing_sample.pan.value, &end_pan.l, &end_pan.r);
		end_pan.l *= end_volume * playing_sample.volume.value;
		end_pan.r *= end_volume * playing_sample.volume.value;

		//figure out a step to add at each sample so that pan will move smoothly from start to end:
		LR pan = start_pan;
		LR pan_step;
		pan_step.l = (end_pan.l - start_pan.l) / MIX_SAMPLES;
		pan_step.r = (end_pan.r - start_pan.r) / MIX_SAMPLES;

		assert(playing_sample.i < playing_sample.data.size());

		for (uint32_t i = 0; i < MIX_SAMPLES; ++i) {
			//mix one sample based on current pan values:
			buffer[i].l += pan.l * playing_sample.data[playing_sample.i];
			buffer[i].r += pan.r * playing_sample.data[playing_sample.i];

			//update position in sample:
			playing_sample.i += 1;
			if (playing_sample.i == playing_sample.data.size()) {
				break;
			}

			//update pan values:
			pan.l += pan_step.l;
			pan.r += pan_step.r;
		}

		if (playing_sample.i >= playing_sample.data.size()) { //sample has finished
		 	playing_sample.stopped = true;
			//erase from list:
			auto old = si;
			++si;
			playing_samples.erase(old);
		} else {
			++si;
		}
	}

	/*//DEBUG: report output power:
	float max_power = 0.0f;
	for (uint32_t s = 0; s < MIX_SAMPLES; ++s) {
		max_power = std::max(max_power, (buffer[s].l * buffer[s].l + buffer[s].r * buffer[s].r));
	}
	std::cout << "Max Power: " << std::sqrt(max_power) << "; playing samples: " << playing_samples.size() << std::endl; //DEBUG
	*/

}


