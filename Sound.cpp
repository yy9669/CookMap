#include "Sound.hpp"

#include <SDL.h>

#include <list>
#include <cassert>

//------------------------ internals --------------------------------

constexpr uint32_t const AUDIO_RATE = 48000;
constexpr uint32_t const MIX_SAMPLES = 1024;

//global volume control:
Sound::Ramp< float > Sound::volume;

//list of all currently playing samples:
std::list< std::shared_ptr< Sound::PlayingSample > > playing_samples;

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

}


