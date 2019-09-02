#include "Sprite.hpp"

#include "GL.hpp"
#include "read_write_chunk.hpp"
#include "load_save_png.hpp"

#include <fstream>

SpriteAtlas::SpriteAtlas(std::string const &filebase) {
	std::string png_path = filebase + ".png";
	atlas_path = filebase + ".atlas";

	// ----- load the texture data -----
	std::vector< glm::u8vec4 > tex_data;
	load_png(png_path, &tex_size, &tex_data, LowerLeftOrigin);

	//upload the texture data to the GPU:

	//generate a new texture object name:
	glGenTextures(1, &tex);

	//bind the new texture object:
	glBindTexture(GL_TEXTURE_2D, tex);

	//upload pixel data:
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_size.x, tex_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data.data());

	//set filtering and wrapping parameters:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//If you were doing pixel art, you'd probably want to filter like this:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	//For smoother artwork, this filtering makes more sense:
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glGenerateMipmap(GL_TEXTURE_2D);

	//unbind the texture object:
	glBindTexture(GL_TEXTURE_2D, 0);

	// ----- load the sprite location data -----

	//read from atlas_path in binary mode:
	std::ifstream in(atlas_path, std::ios::binary);

	//sprite atlas is stored as two chunks:
	// (1) a 'str0' chunk with string data:
	std::vector< char > strings;

	read_chunk(in, "str0", &strings);

	// (2) a 'spr0' chunk with sprite data:
	struct SpriteData {
		uint32_t name_begin, name_end;
		glm::vec2 min_px;
		glm::vec2 max_px;
		glm::vec2 anchor_px;
	};
	std::vector< SpriteData > datas;

	read_chunk(in, "spr0", &datas);

	//actually create Sprite objects from the data and insert into the lookup table:

	//let the hash table know how many elements we are going to insert (could save a re-allocation of the backings store):
	sprites.reserve(datas.size());

	//actually insert all items into the data table:
	for (auto const &data : datas) {

		//first, use the name_begin and name_end fields to read the sprite's name from the strings table:
		if (data.name_begin > data.name_end || data.name_end > strings.size()) {
			throw std::runtime_error("Invalid name in sprite atlas '" + atlas_path + "'.");
		}
		std::string name(strings.begin() + data.name_begin, strings.begin() + data.name_end);

		//then populate a new Sprite struct using the data:
		Sprite sprite;
		sprite.min_px = data.min_px;
		sprite.max_px = data.max_px;
		sprite.anchor_px = data.anchor_px;

		//finally, insert into the sprites lookup table:
		auto ret = sprites.insert(std::make_pair(name, sprite));
		if (!ret.second) {
			throw std::runtime_error("Sprite with duplicate name '" + name + "' in sprite atlas '" + atlas_path + "',");
		}
	}
}

SpriteAtlas::~SpriteAtlas() {
	glDeleteTextures(1, &tex);
	tex = 0;
}

Sprite const &SpriteAtlas::lookup(std::string const &name) const {
	auto f = sprites.find(name);
	if (f == sprites.end()) {
		throw std::runtime_error("Sprite of name '" + name + "' not found in atlas '" + atlas_path + "'.");
	}
	return f->second;
}
