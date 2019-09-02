#pragma once

/*
 * A sprite is a rectangular area in a atlas texture.
 * Sprites are loaded by creating a 'SpriteAtlas' (which both loads a texture
 * image and sprite position metadata); you can then look up individual
 * 'Sprite's in the 'SpriteAtlas' using its lookup() function.
 */

#include "GL.hpp"

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>

struct Sprite {
	//Sprites are rectangles in an atlas texture:
	glm::vec2 min_px; //position of lower left corner (in pixels; ll-origin)
	glm::vec2 max_px; //position of upper right corner (in pixels; ll-origin)
	glm::vec2 anchor_px; //position of 'anchor' (in pixels; ll-origin)

	//NOTE:
	//The 'anchor' is the "center" or "pivot point" of the sprite --
	// a value defined when authoring the sprite which is used as the
	// position of the sprite when drawing.

	//Generally, you set it to something meaningful like the center of mass
	// or the position of the feet.
};

struct SpriteAtlas {
	//load from filebase.png and filebase.atlas:
	SpriteAtlas(std::string const &filebase);
	~SpriteAtlas();

	//look up sprite in list of loaded sprites:
	// throws an error if name is missing
	Sprite const &lookup(std::string const &name) const;

	//this is the atlas texture; used when drawing sprites:
	GLuint tex = 0;
	glm::uvec2 tex_size = glm::uvec2(0);

	//---- internal data ---

	//table of loaded sprites, sorted by name:
	std::unordered_map< std::string, Sprite > sprites;

	//path to atlas, stored for debugging purposes:
	std::string atlas_path;
};

