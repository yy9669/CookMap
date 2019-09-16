#include "load_wav.hpp"

#include <SDL.h>

#include <iostream>
#include <cassert>
#include <algorithm>

constexpr uint32_t AUDIO_RATE = 48000;

void load_wav(std::string const &filename, std::vector< float > *data_) {
	assert(data_);
	auto &data = *data_;

	SDL_AudioSpec audio_spec;
	Uint8 *audio_buf = nullptr;
	Uint32 audio_len = 0;

	SDL_AudioSpec *have = SDL_LoadWAV(filename.c_str(), &audio_spec, &audio_buf, &audio_len);
	if (!have) {
		throw std::runtime_error("Failed to load WAV file '" + filename + "'; SDL says \"" + std::string(SDL_GetError()) + "\"");
	}

	//based on the SDL_AudioCVT example in the docs: https://wiki.libsdl.org/SDL_AudioCVT
	SDL_AudioCVT cvt;
	SDL_BuildAudioCVT(&cvt, have->format, have->channels, have->freq, AUDIO_F32SYS, 1, AUDIO_RATE);
	if (cvt.needed) {
		std::cout << "WAV file '" + filename + "' didn't load as " + std::to_string(AUDIO_RATE) + " Hz, float32, mono; converting." << std::endl;
		cvt.len = audio_len;
		cvt.buf = (Uint8 *)SDL_malloc(cvt.len * cvt.len_mult);
		SDL_memcpy(cvt.buf, audio_buf, audio_len);
		SDL_ConvertAudio(&cvt);
		int final_size = cvt.len_cvt;
		assert(final_size >= 0 && final_size <= cvt.len && "Converted audio should fit in buffer.");
		assert(final_size % 4 == 0 && "Converted audio should consist of 4-byte elements.");
		data.assign(reinterpret_cast< float * >(cvt.buf), reinterpret_cast< float * >(cvt.buf + final_size));
		SDL_free(cvt.buf);
	} else {
		data.assign(reinterpret_cast< float * >(audio_buf), reinterpret_cast< float * >(audio_buf + audio_len));
	}
	SDL_FreeWAV(audio_buf);

	float min = 0.0f;
	float max = 0.0f;
	for (auto d : data) {
		min = std::min(min, d);
		max = std::max(max, d);
	}
	std::cout << "Range: " << min << ", " << max << std::endl;
}
