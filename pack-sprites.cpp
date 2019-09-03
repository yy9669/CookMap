#include "load_save_png.hpp"
#include "read_write_chunk.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <list>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <random>
#include <fstream>

/*
 *pack sprites into an atlas texture and save an info file.
 * reads list of sprites from command line arguments.
 * sprites should be named "name_ax_ay.png" where ax and ay are the sprite anchor positions (relative to a top-left origin)
 *
 */

//helper to underscore-decode a name; defined at the end of this file:
std::string decode_name(std::string const &name);

int main(int argc, char **argv) {
#ifdef _WIN32
	try { //windows doesn't print nice errors for unhandled exceptions, so we need to.
#endif
	if (argc < 2) {
		std::cerr << "Usage:\n\t./pack-sprites <outname> [sprite1.png] [sprite2.png] ...\n";
		std::cerr << " will create \"outname.atlas\" and \"outname.png\" from sprites sprite1.png, ...\n";
		std::cerr << " sprites should be named \"name_ax_ay.png\" where \"name\" is the name written into the atlas and ax and ay are the anchor positions in the image in pixel coordinates with a top-left origin.\n";
		std::cerr << " NOTE: name will be transformed as follows:\n";
		std::cerr << "   \"__\" => \"_\" (double underscore to single)\n";
		std::cerr << "   \"_a\" => \"_A\" (letter after underscore upper-cased)\n";
		std::cerr << "   \"_0HHHHHH\" => \"_x\" (0-prefixed utf8 hex encoding after underscore decoded)\n";
		std::cerr.flush();
		return 1;
	}
	uint32_t margin = 1; //space to leave between sprites
	std::string outname = argv[1];

	if (outname.size() > 4 && outname.substr(outname.size()-4) == ".png") {
		std::cerr << "ERROR: your output file (" << outname << ") shouldn't have an .png extension." << std::endl;
		return 1;
	}

	struct Sprite {
		glm::uvec2 size = glm::uvec2(0); //size of sprite, in pixels
		std::vector< glm::u8vec4 > data; //pixel data for sprite
		std::string name = ""; //name for in-game lookup
		glm::vec2 anchor = glm::vec2(0.0f); //position of anchor in sprite -- pixel coordinates, upper-left origin
	};

	std::vector< Sprite > sprites;
	sprites.reserve(argc - 2); //pre-allocate space for sprites
	for (int i = 2; i < argc; ++i) {
		//add a new sprite to the list and make a handy reference to it:
		sprites.emplace_back();
		Sprite &sprite = sprites.back();

		std::string filepath = argv[i];

		//actually load the sprite:
		load_png(filepath, &sprite.size, &sprite.data, LowerLeftOrigin);

		//parse filename to figure out anchor/name:

		std::string filename;
		{ //compute filename by getting just the part after the '/' or '\':
			auto last_sep = filepath.find_last_of("/\\");
			if (last_sep != std::string::npos) {
				filename = filepath.substr(last_sep+1);
			} else {
				filename = filepath;
			}
		}

		//remove the _ax_ay.ext:
		{
			std::string remain;
			auto dot = filename.rfind('.');
			if (dot != std::string::npos) {
				remain = filename.substr(0,dot);
			} else {
				remain = filename;
				std::cerr << "WARNING: input sprite without file extension -- will try to load as .png anyway." << std::endl;
			}
			auto underscore = remain.rfind('_');
			if (underscore != std::string::npos) {
				std::istringstream ay_str(remain.substr(underscore + 1));
				char temp;
				if (!(ay_str >> sprite.anchor.y) || (ay_str >> temp)) {
					std::cerr << "Failed to parse _ay part as float: \"" << filepath << "\"" << std::endl;
					return 1;
				}
				remain = remain.substr(0, underscore);
			} else {
				std::cerr << "ERROR: input sprite without a _ay part: \"" << filepath << "\"." << std::endl;
				return 1;
			}
			underscore = remain.rfind('_');
			if (underscore != std::string::npos) {
				std::istringstream ax_str(remain.substr(underscore + 1));
				char temp;
				if (!(ax_str >> sprite.anchor.x) || (ax_str >> temp)) {
					std::cerr << "Failed to parse _ax part as float: \"" << filepath << "\"" << std::endl;
					return 1;
				}
				try {
					sprite.name = decode_name(remain.substr(0, underscore));
				} catch (std::exception &e) {
					std::cerr << "ERROR: failed to decode sprite name:\n" << e.what() << std::endl;
					return 1;
				}
			} else {
				std::cerr << "ERROR: input sprite without a _ax part: \"" << filepath << "\"." << std::endl;
				return 1;
			}
		}
	}

	std::cout << "Will pack the following sprites with margin " << margin << " and save into " << outname << ".png and " << outname << ".atlas :\n";
	for (auto const &sprite : sprites) {
		std::cout << "\t\"" << sprite.name << "\" " << sprite.size.x << "x" << sprite.size.y << " with anchor at " << sprite.anchor.x << ", " << sprite.anchor.y << "\n";
	}
	std::cout.flush();

	//----------------------------------
	//sort items (in order to get consistent cross-platform behavior when run as `pack-sprites out *`):
	std::sort(sprites.begin(), sprites.end(), [](Sprite const &a, Sprite const &b){
		return a.name < b.name;
	});
	//quick sanity check -- should not have duplicate names:
	for (uint32_t i = 1; i < sprites.size(); ++i) {
		if (sprites[i-1].name == sprites[i].name) {
			std::cerr << "ERROR: have two sprites with the same name -- \"" << sprites[i].name << "\" -- this is not allowed by the sprite lookup code." << std::endl;
			return 1;
		}
	}

	//----------------------------------
	//will build a packing into this structure:

	struct Packing {
		glm::uvec2 size = glm::uvec2(1,1); //overall packing size
		std::vector< glm::uvec2 > lls; //sprite lower-left positions
	};
	

	//----------------------------------
	//First-fit packing:

	auto try_first_fit = [&sprites,&margin](bool randomize) -> Packing {
		Packing pk;
		pk.size = glm::uvec2(1);
		pk.lls.resize(sprites.size(), glm::uvec2(-1U));

		glm::uvec2 &size = pk.size; //for convenience

		//optimistic initial sizing based on size of largest sprite:
		for (auto const &sprite : sprites) {
			while (size.x < sprite.size.x + 2*margin) size.x *= 2;
			while (size.y < sprite.size.y + 2*margin) size.y *= 2;
		}

		//pick insertion order for sprites:
		std::vector< uint32_t > order;
		order.reserve(sprites.size());
		for (uint32_t i = 0; i < sprites.size(); ++i) {
			order.emplace_back(i);
		}
		if (randomize) {
			//shuffle insertion order:
			static std::mt19937 mt(0x12345678); //seed supplied -- want deterministic cross-platform behavior
			for (uint32_t i = 0; i < order.size(); ++i) {
				std::swap(order[i], order[i + mt() % (order.size()-i)]);
			}
		} else {
			//order sprites by maximum dimension (largest-first):
			std::stable_sort(order.begin(), order.end(), [&sprites](uint32_t a, uint32_t b){
				return std::max(sprites[a].size.x, sprites[a].size.y) > std::max(sprites[b].size.x, sprites[b].size.y);
			});
		}

		//add sprites incrementally:
		for (auto oi = order.begin(); oi != order.end(); /* later */) {
			Sprite const &sprite = sprites[*oi];
			glm::uvec2 &ll = pk.lls[*oi];

			//compute occupancy map for all earlier sprites:
			std::vector< bool > filled(size.x*size.y, false);
			auto fill_rect = [&filled,&size](glm::uvec2 const &ll, glm::uvec2 const &sz) {
				for (uint32_t y = 0; y < sz.y; ++y) {
					for (uint32_t x = 0; x < sz.x; ++x) {
						filled[(ll.y+y)*size.x+(ll.x+x)] = true;
					}
				}
			};
			for (auto oi2 = order.begin(); oi2 != oi; ++oi2) {
				fill_rect(pk.lls[*oi2], sprites[*oi2].size);
			}

			//compute a snazzy lookup table for fast "free rectangle" query:
			//empty_count[y*size.x+x] == empty cells with cx <= x and cy <= y
			std::vector< uint32_t > empty_count(size.x*size.y, 0);
	
			empty_count[0*size.x+0] = (filled[0*size.x+0] ? 0 : 1);
			for (uint32_t x = 1; x < size.x; ++x) {
				empty_count[0*size.x+x] = empty_count[0*size.x+(x-1)] + (filled[0*size.x+x] ? 0 : 1);
			}
			for (uint32_t y = 1; y < size.y; ++y) {
				empty_count[y*size.x+0] = empty_count[(y-1)*size.x+0] + (filled[y*size.x+0] ? 0 : 1);
				for (uint32_t x = 1; x < size.x; ++x) {
					//add up counts to the left and below, remove the (double-counted) part to the below-left:
					empty_count[y*size.x+x] = empty_count[(y-1)*size.x+x] + empty_count[y*size.x+(x-1)] - empty_count[(y-1)*size.x+(x-1)] + (filled[y*size.x+x] ? 0 : 1);
				}
			}

			//use lookup table to compute how many free cells exist inside a given rectangle:
			auto get_empty_count = [&empty_count, &size](glm::uvec2 const &ll, glm::uvec2 const &sz) -> uint32_t {
				assert(ll.x < size.x && ll.y < size.y && ll.x + sz.x <= size.x && ll.y + sz.y <= size.y);
				if (sz.x == 0 || sz.y == 0) return 0U;
				//start with everything to the lower left of max point in rectangle:
				uint32_t ret = empty_count[(ll.y+sz.y-1)*size.x+(ll.x+sz.x-1)];
				if (ll.x > 0 && ll.y > 0) {
					//double things to the lower left of the min point (because they will get removed twice):
					ret += empty_count[(ll.y-1)*size.x+(ll.x-1)];
				}
				if (ll.x > 0) {
					//remove things to the left of the rectangle:
					ret -= empty_count[(ll.y+sz.y-1)*size.x+(ll.x-1)];
				}
				if (ll.y > 0) {
					//remove things below of the rectangle:
					ret -= empty_count[(ll.y-1)*size.x+(ll.x+sz.x-1)];
				}
				return ret;
			};

#ifdef PARANOIA
			{ //PARANOIA: test a few random sums:
				static std::mt19937 mt(0xdeadbeef);
				for (uint32_t iter = 0; iter < 100; ++iter) {
					glm::uvec2 a(mt() % size.x, mt() % size.y);
					glm::uvec2 b(mt() % size.x, mt() % size.y);
					glm::uvec2 min = glm::min(a,b);
					glm::uvec2 max = glm::max(a,b);
					uint32_t from_table = get_empty_count(min, max + glm::uvec2(1) - min);
					uint32_t from_counting = 0;
					for (uint32_t x = min.x; x <= max.x; ++x) {
						for (uint32_t y = min.y; y <= max.y; ++y) {
							from_counting += (filled[y*size.x+x] ? 0 : 1);
						}
					}
					assert(from_counting == from_table);
				}
			}
#endif //PARANOIA

			//TODO: some sort of fancy spiral pattern?
			glm::uvec2 sz = sprite.size + 2U * glm::uvec2(margin);
			glm::uvec2 min_ll = glm::uvec2(0);
			glm::uvec2 max_ll = size - sz;

			ll = glm::uvec2(-1U);
			for (uint32_t y = min_ll.y; y <= max_ll.y; ++y) {
				for (uint32_t x = min_ll.x; x <= max_ll.x; ++x) {
					if (get_empty_count(glm::uvec2(x,y), sz) == sz.x * sz.y) {
						ll = glm::uvec2(x,y);
						break;
					}
				}
				if (ll != glm::uvec2(-1)) break;
			}

			if (ll == glm::uvec2(-1)) {
				//std::cout << "Failed to pack at " << size.x << "x" << size.y;
				if (size.x <= size.y) size.x *= 2;
				else size.y *= 2;
				//std::cout << " increasing to " << size.x << "x" << size.y << std::endl;
				continue; //retry (just) this sprite
			}

			ll += glm::uvec2(margin);
			//std::cout << "Packed \"" << sprite.name << "\" at " << ll.x << "," << ll.y << std::endl;
			assert((ll + sprite.size + glm::uvec2(margin)).x <= size.x && (ll + sprite.size + glm::uvec2(margin)).y <= size.y);

			++oi; //go to next sprite
		}

		return pk;
	};

	//start with a simple packing:
	std::cout << "Doing first packing..."; std::cout.flush();
	Packing packing = try_first_fit(false);
	std::cout << " done." << std::endl;
	std::cout << "Got size " << packing.size.x << "x" << packing.size.y << "; trying some random other orders to see if this can be improved... "; std::cout.flush();

	/* this doesn't seem to improve things in general:
	//try some random-order packings to see if any are better:
	for (uint32_t iter = 0; iter < 500; ++iter) {
		Packing test = try_first_fit(true);
		if (test.size.x * test.size.y < packing.size.x * packing.size.y) {
			packing = test;
			std::cout << " decreased to " << packing.size.x << "x" << packing.size.y << " on step " << iter << std::endl;
		}
	}
	std::cout << " done." << std::endl;
	*/

	assert(packing.lls.size() == sprites.size());

	//render final arrangement:
	std::cout << "Building output image..."; std::cout.flush();
	std::vector< glm::u8vec4 > data(packing.size.x*packing.size.y, glm::u8vec4(0x00, 0x00, 0x00, 0x00));
	for (uint32_t i = 0; i < sprites.size(); ++i) {
		Sprite const &sprite = sprites[i];
		glm::uvec2 ll = packing.lls[i];
		for (uint32_t y = 0; y < sprite.size.y; ++y) {
			for (uint32_t x = 0; x < sprite.size.x; ++x) {
				auto &px = data[(ll.y+y)*packing.size.x+(ll.x+x)];
				assert(px == glm::u8vec4(0x00, 0x00, 0x00, 0x00));
				assert(sprite.data.size() == sprite.size.x * sprite.size.y);
				px = sprite.data[y*sprite.size.x+x];
			}
		}
	}
	std::cout << " done." << std::endl;

	//TODO: Eventually, add utilities to set transparent pixels to the color of the nearest opaque pixel

	std::cout << "Saving " << outname << ".png ..."; std::cout.flush();
	save_png(outname + ".png", packing.size, data.data(), LowerLeftOrigin);
	std::cout << " done." << std::endl;

	std::cout << "Saving " << outname << ".atlas ..."; std::cout.flush();
	{ //store ".atlas" file describing final image:
		std::vector< char > strings;

		struct SpriteData {
			uint32_t name_begin, name_end;
			glm::vec2 min_px;
			glm::vec2 max_px;
			glm::vec2 anchor_px;
		};
		std::vector< SpriteData > datas;

		for (uint32_t si = 0; si < sprites.size(); ++si) {
			Sprite const &sprite = sprites[si];
			glm::uvec2 const &ll = packing.lls[si];

			datas.emplace_back();
			auto &data = datas.back();
			data.name_begin = uint32_t(strings.size());
			strings.insert(strings.end(), sprite.name.begin(), sprite.name.end());
			data.name_end = uint32_t(strings.size());
			data.min_px = glm::vec2(ll);
			data.max_px = glm::vec2(ll + sprite.size);
			//convert anchor to ll-origin:
			data.anchor_px = glm::vec2(
				ll.x + sprite.anchor.x,
				ll.y + sprite.size.y - sprite.anchor.y
			);
		}

		std::ofstream out(outname + ".atlas", std::ios::binary);
		write_chunk("str0", strings, &out);
		write_chunk("spr0", datas, &out);
	}
	std::cout << " done." << std::endl;

	return 0;
#ifdef _WIN32
	} catch (std::exception &e) {
		std::cerr << "UNHANDLED EXCEPTION:\n" << e.what() << std::endl;
		return 1;
	}
#endif
	
}



std::string decode_name(std::string const &name) {
	std::string out;
	for (uint32_t i = 0; i < name.size(); ++i) {
		if ((name[i] >= 'a' && name[i] <= 'z')
		 || (name[i] >= '0' && name[i] <= '9')
		 || name[i] == '-') {
			out += name[i];
		} else if (name[i] == '_') {
			if (i + 1 == name.size()) throw std::runtime_error("Can't name-decode _ at end of string.");
			if (name[i+1] == '_') {
				out += "_";
				i += 1;
			} else if (name[i+1] >= 'a' && name[i+1] <= 'z') {
				out += char(int(name[i+1])+(int('A')-int('a')));
				i += 1;
			} else if (name[i+1] == '0') {
				if (i + 3 >= name.size()) throw std::runtime_error("Can't name-decode _0 without at least two more characters.");
				auto byte_from_hex = [](char h, char l) {
					uint8_t val = 0;
					if ('A' <= h && h <= 'F') val += 10 + (h - 'A');
					else if ('a' <= h && h <= 'f') val += 10 + (h - 'a');
					else if ('0' <= h && h <= '9') val += (h - '0');
					else throw std::runtime_error("Character '" + std::to_string(h) + "' is not hex.");
					val *= 16;
					if ('A' <= l && l <= 'F') val += 10 + (l - 'A');
					else if ('a' <= l && l <= 'f') val += 10 + (l - 'a');
					else if ('0' <= l && l <= '9') val += (l - '0');
					else throw std::runtime_error("Character '" + std::to_string(l) + "' is not hex.");
					return val;
				};
				uint8_t val = byte_from_hex(name[i+2], name[i+3]);
				if        ((val & 0b1000'0000) == 0b0000'0000) { //one-byte value starts 0xxxxxxx
					out += char(val);
					i += 3;
				} else if ((val & 0b1110'0000) == 0b1100'0000) { //two-byte value starts 110xxxxx
					if (i + 5 >= name.size()) throw std::runtime_error("Ran out of characters decoding _0xxxx");
					uint8_t val1 = byte_from_hex(name[i+4],name[i+5]);
					if ((val1 & 0b1100'0000) != 0b1000'0000) throw std::runtime_error("Invalid utf8 decoding _0xxxx");
					out += char(val);
					out += char(val1);
					i += 5;
				} else if ((val & 0b1111'0000) == 0b1110'0000) { //three-byte value starts 1110xxxx
					if (i + 7 >= name.size()) throw std::runtime_error("Ran out of characters decoding _0xxxxxx");
					uint8_t val1 = byte_from_hex(name[i+4],name[i+5]);
					uint8_t val2 = byte_from_hex(name[i+6],name[i+7]);
					if ((val1 & 0b1100'0000) != 0b1000'0000) throw std::runtime_error("Invalid utf8 decoding _0xxxxxx");
					if ((val2 & 0b1100'0000) != 0b1000'0000) throw std::runtime_error("Invalid utf8 decoding _0xxxxxx");
					out += char(val);
					out += char(val1);
					out += char(val2);
					i += 7;
				} else if ((val & 0b1111'1000) == 0b1111'0000) { //four-byte value starts 11110xxx
					if (i + 9 >= name.size()) throw std::runtime_error("Ran out of characters decoding _0xxxxxxxx");
					uint8_t val1 = byte_from_hex(name[i+4],name[i+5]);
					uint8_t val2 = byte_from_hex(name[i+6],name[i+7]);
					uint8_t val3 = byte_from_hex(name[i+8],name[i+9]);
					if ((val1 & 0b1100'0000) != 0b1000'0000) throw std::runtime_error("Invalid utf8 decoding _0xxxxxxxx");
					if ((val2 & 0b1100'0000) != 0b1000'0000) throw std::runtime_error("Invalid utf8 decoding _0xxxxxxxx");
					if ((val3 & 0b1100'0000) != 0b1000'0000) throw std::runtime_error("Invalid utf8 decoding _0xxxxxxxx");
					out += char(val);
					out += char(val1);
					out += char(val2);
					out += char(val3);
					i += 9;
				} else {
					throw std::runtime_error("Invalid utf8 decoding _0");
				}
			} else {
				throw std::runtime_error("Invalid character '" + std::to_string(name[i+1]) + "' after underscore.");
			}
		} else {
			throw std::runtime_error("Invalid character '" + std::to_string(name[i]) + "' in underscore-encoded name.");
		}
	}
	return out;
}
