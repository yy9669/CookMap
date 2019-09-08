#include "load_opus.hpp"

#include "opusfile.h"

#include <cassert>
#include <memory>

void load_opus(std::string const &filename, std::vector< float > *data_) {
	assert(data_);
	auto &data = *data_;
	data.clear();

	//will hold opusfile * int a std::unique_ptr so that it will automatically be deleted:
	int err = 0;
	std::unique_ptr< OggOpusFile, decltype(&op_free) > op(
		op_open_file(filename.c_str(), &err), //pointer to hold
		op_free //deletion function
	);
	if (err != 0) {
		throw std::runtime_error("opusfile error " + std::to_string(err) + " opening \"" + filename + "\".");
	}

	for (;;) {
		std::vector< float > pcm(2*48000*2, 0.0f);
		int ret = op_read_float_stereo(op.get(), pcm.data(), pcm.size());
		if (ret >= 0) {
			//positive return values are the number of samples read per channel; copy into data:
			data.insert(data.end(), pcm.begin(), pcm.begin() + 2*ret);
			if (ret == 0) break;
		} else {
			throw std::runtime_error("opusfile read error " + std::to_string(ret) + " reading \"" + filename + "\".");
		}
	}


}
