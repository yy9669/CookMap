#include "DrawSprites.hpp"

#include "ColorTextureProgram.hpp"
#include "Load.hpp"

#include "GL.hpp"
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

//for glm::to_string():
#include <glm/gtx/string_cast.hpp>

#include <algorithm>

//All DrawSprites instances share a vertex array object and vertex buffer, initialized at load time:

//n.b. declared static so they don't conflict with similarly named global variables elsewhere:
static GLuint vertex_buffer = 0;
static GLuint vertex_buffer_for_color_texture_program = 0;

Load< void > setup_buffers(LoadTagDefault, [](){
	//you may recognize this init code from PongMode.cpp in base0:

	{ //set up vertex buffer:
		glGenBuffers(1, &vertex_buffer);
		//for now, buffer will be un-filled.
	}

	{ //vertex array mapping buffer for color_texture_program:
		//ask OpenGL to fill vertex_buffer_for_color_texture_program with the name of an unused vertex array object:
		glGenVertexArrays(1, &vertex_buffer_for_color_texture_program);

		//set vertex_buffer_for_color_texture_program as the current vertex array object:
		glBindVertexArray(vertex_buffer_for_color_texture_program);

		//set vertex_buffer as the source of glVertexAttribPointer() commands:
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

		//set up the vertex array object to describe arrays of PongMode::Vertex:
		glVertexAttribPointer(
			color_texture_program->Position_vec4, //attribute
			2, //size
			GL_FLOAT, //type
			GL_FALSE, //normalized
			sizeof(DrawSprites::Vertex), //stride
			(GLbyte *)0 + offsetof(DrawSprites::Vertex, Position) //offset
		);
		glEnableVertexAttribArray(color_texture_program->Position_vec4);
		//[Note that it is okay to bind a vec3 input to a vec4 attribute -- the w component will be filled with 1.0 automatically]

		glVertexAttribPointer(
			color_texture_program->TexCoord_vec2, //attribute
			2, //size
			GL_FLOAT, //type
			GL_FALSE, //normalized
			sizeof(DrawSprites::Vertex), //stride
			(GLbyte *)0 + offsetof(DrawSprites::Vertex, TexCoord) //offset
		);
		glEnableVertexAttribArray(color_texture_program->TexCoord_vec2);

		glVertexAttribPointer(
			color_texture_program->Color_vec4, //attribute
			4, //size
			GL_UNSIGNED_BYTE, //type
			GL_TRUE, //normalized
			sizeof(DrawSprites::Vertex), //stride
			(GLbyte *)0 + offsetof(DrawSprites::Vertex, Color) //offset
		);
		glEnableVertexAttribArray(color_texture_program->Color_vec4);

		//done referring to vertex_buffer, so unbind it:
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//done setting up vertex array object, so unbind it:
		glBindVertexArray(0);
	}

	GL_ERRORS(); //PARANOIA: make sure nothing strange happened during setup
});


DrawSprites::DrawSprites(
	SpriteAtlas const &atlas_,
	glm::vec2 const &view_min_, glm::vec2 const &view_max_,
	glm::uvec2 const &drawable_size_,
	DrawSprites::AlignMode mode_
	) :
	atlas(atlas_),
	view_min(view_min_), view_max(view_max_),
	drawable_size(drawable_size_),
	mode(mode_) {

	glm::vec2 window_min, window_max;

	if (mode == AlignPixelPerfect) {
		//figure out the largest view can be while still mapping 1-1 to pixels:
		float scale = std::min(
			drawable_size.x / (view_max.x-view_min.x),
			drawable_size.y / (view_max.y-view_min.y)
		);
		//unless scaling down, make scale an integer (pixels -> pixels):
		if (scale > 1.0f) scale = std::floor(scale);

		//map view center pixel's lower-left to center pixel's lower-left:
		glm::vec2 offset =
			- glm::vec2( //map view center pixel's lower-left:
				std::floor(scale * (view_max.x + view_min.x)*0.5f),
				std::floor(scale * (view_max.y + view_min.y)*0.5f)
			) + glm::vec2( //to window center pixel's lower-left:
				std::floor(0.5f * drawable_size.x),
				std::floor(0.5f * drawable_size.y)
			) - glm::vec2( //.. relative to window center:
				0.5f * drawable_size.x,
				0.5f * drawable_size.y
			);

		to_clip = glm::mat4( //n.b. column major(!)
			scale * 2.0f / float(drawable_size.x), 0.0f, 0.0f, 0.0f,
			0.0f, scale * 2.0f / float(drawable_size.y), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			2.0f / float(drawable_size.x) * offset.x, 2.0f / float(drawable_size.y) * offset.y, 0.0f, 1.0f
		);

	} else {
		//if view_size.x / view_size.y < drawable_size.x / drawable_size.y...
		if ((view_max.x - view_min.x) * drawable_size.y < drawable_size.x * (view_max.y - view_min.y)) {
			//...need to stretch wider to match aspect:
			float w = (view_max.y - view_min.y) * float(drawable_size.x) / float(drawable_size.y);
			window_min.x = 0.5f * (view_min.x + view_max.x) - 0.5f * w;
			window_max.x = 0.5f * (view_min.x + view_max.x) + 0.5f * w;
			window_min.y = view_min.y;
			window_max.y = view_max.y;
		} else {
			//...need to stretch taller to match aspect:
			window_min.x = view_min.x;
			window_max.x = view_max.x;
			float h = (view_max.x - view_min.x) * float(drawable_size.y) / float(drawable_size.x);
			window_min.y = 0.5f * (view_min.y + view_max.y) - 0.5f * h;
			window_max.y = 0.5f * (view_min.y + view_max.y) + 0.5f * h;
		}

		glm::vec2 scale = glm::vec2(2.0f) / (window_max - window_min);
		to_clip = glm::mat4( //n.b. column major(!)
			scale.x, 0.0f, 0.0f, 0.0f,
			0.0f, scale.y, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			scale.x * -0.5f * (window_min.x + window_max.x), scale.y * -0.5f * (window_min.y + window_max.y), 0.0f, 1.0f
		);
	}

	//DEBUG: std::cout << glm::to_string(to_clip) << std::endl;
}

void DrawSprites::draw(Sprite const &sprite, glm::vec2 const &center, float scale, glm::u8vec4 const &tint) {
	glm::vec2 min = center + scale * (sprite.min_px - sprite.anchor_px);
	glm::vec2 max = center + scale * (sprite.max_px - sprite.anchor_px);
	glm::vec2 min_tc = sprite.min_px / glm::vec2(atlas.tex_size);
	glm::vec2 max_tc = sprite.max_px / glm::vec2(atlas.tex_size);

	if (mode == AlignPixelPerfect) {
		//nudge min/max so that pixels line up just ~just so~
		//notably, want nearest pixel center to anchor to line up on a pixel center:
		glm::vec2 c = center;
		glm::vec2 ofs = (glm::floor(sprite.anchor_px) + glm::vec2(0.5f)) - sprite.anchor_px;
		//move c to nearest pixel center:
		c += ofs * scale;
		//make sure c is on a pixel center:
		c = glm::floor(c) + glm::vec2(0.5f);
		//move c back to anchor:
		c -= ofs * scale;

		//recompute sprite location:
		min = c + scale * (sprite.min_px - sprite.anchor_px);
		max = c + scale * (sprite.max_px - sprite.anchor_px);
	}

	//you may recognize this from draw_rectangle in base0:
	//split rectangle into two triangles:
	attribs.emplace_back(glm::vec2(min.x,min.y), glm::vec2(min_tc.x,min_tc.y), tint);
	attribs.emplace_back(glm::vec2(max.x,min.y), glm::vec2(max_tc.x,min_tc.y), tint);
	attribs.emplace_back(glm::vec2(max.x,max.y), glm::vec2(max_tc.x,max_tc.y), tint);

	attribs.emplace_back(glm::vec2(min.x,min.y), glm::vec2(min_tc.x,min_tc.y), tint);
	attribs.emplace_back(glm::vec2(max.x,max.y), glm::vec2(max_tc.x,max_tc.y), tint);
	attribs.emplace_back(glm::vec2(min.x,max.y), glm::vec2(min_tc.x,max_tc.y), tint);

}

void DrawSprites::draw_text(std::string const &text, glm::vec2 const &anchor, float scale, glm::u8vec4 const &tint, glm::vec2 *anchor_out) {
	glm::vec2 moving_anchor = anchor;
	for (size_t pos = 0; pos < text.size(); pos++){
		Sprite const &chr = atlas.lookup(text.substr(pos,1));
		draw(chr, moving_anchor, scale, tint);
		moving_anchor.x += (chr.max_px.x - chr.min_px.x + 1) * scale;
	}

	if (anchor_out) {
		*anchor_out = moving_anchor;
	}
}

void DrawSprites::get_text_extents(std::string const &text, glm::vec2 const &anchor, float scale, glm::vec2 *min_, glm::vec2 *max_) {
	assert(min_);
	auto &min = *min_;
	assert(max_);
	auto &max = *max_;

	min = glm::vec2(std::numeric_limits< float >::infinity());
	max = glm::vec2(-std::numeric_limits< float >::infinity());

	glm::vec2 moving_anchor = anchor;
	for (size_t pos = 0; pos < text.size(); pos++){
		Sprite const &chr = atlas.lookup(text.substr(pos,1));
		min = glm::min(min, moving_anchor + (chr.min_px - chr.anchor_px) * scale);
		max = glm::max(max, moving_anchor + (chr.max_px - chr.anchor_px) * scale);
		moving_anchor.x += (chr.max_px.x - chr.min_px.x + 1) * scale;
	}
}

DrawSprites::~DrawSprites() {
	if (attribs.empty()) return;

	//based on base0's PongMode::draw()

	//upload vertices to vertex_buffer:
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); //set vertex_buffer as current
	glBufferData(GL_ARRAY_BUFFER, attribs.size() * sizeof(attribs[0]), attribs.data(), GL_STREAM_DRAW); //upload attribs array
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//set color_texture_program as current program:
	glUseProgram(color_texture_program->program);

	//upload OBJECT_TO_CLIP to the proper uniform location:
	glUniformMatrix4fv(color_texture_program->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(to_clip));

	//use the mapping vertex_buffer_for_color_texture_program to fetch vertex data:
	glBindVertexArray(vertex_buffer_for_color_texture_program);

	//bind the solid white texture to location zero:
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atlas.tex);

	//run the OpenGL pipeline:
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(attribs.size()));

	//unbind the solid white texture:
	glBindTexture(GL_TEXTURE_2D, 0);

	//reset vertex array to none:
	glBindVertexArray(0);

	//reset current program to none:
	glUseProgram(0);
}

