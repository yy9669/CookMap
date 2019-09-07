#pragma once

#include <string>
#include <vector>

//Load a WAV file as 48kHz floating-point mono; throws on error:
void load_wav(std::string const &filename, std::vector< float > *data);
