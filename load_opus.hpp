#pragma once

#include <string>
#include <vector>

//Load an opus file as 48kHz floating-point mono; throws on error:
void load_opus(std::string const &filename, std::vector< float > *data);
