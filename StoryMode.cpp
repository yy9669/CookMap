#include "StoryMode.hpp"

#include "Sprite.hpp"
#include "DrawSprites.hpp"
#include "Load.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"
#include "MenuMode.hpp"
#include "Sound.hpp"

Sprite const *sprite_left_select = nullptr;
Sprite const *sprite_right_select = nullptr;

Sprite const *sprite_dunes_bg = nullptr;
Sprite const *sprite_dunes_traveller = nullptr;
Sprite const *sprite_dunes_ship = nullptr;

Sprite const *sprite_oasis_bg = nullptr;
Sprite const *sprite_oasis_traveller = nullptr;
Sprite const *sprite_oasis_missing = nullptr;

Sprite const *sprite_hill_bg = nullptr;
Sprite const *sprite_hill_traveller = nullptr;
Sprite const *sprite_hill_missing = nullptr;

Load< SpriteAtlas > sprites(LoadTagDefault, []() -> SpriteAtlas const * {
	SpriteAtlas const *ret = new SpriteAtlas(data_path("the-planet"));

	sprite_left_select = &ret->lookup("text-select-left");
	sprite_right_select = &ret->lookup("text-select-right");

	sprite_dunes_bg = &ret->lookup("dunes-bg");
	sprite_dunes_traveller = &ret->lookup("dunes-traveller");
	sprite_dunes_ship = &ret->lookup("dunes-ship");

	sprite_oasis_bg = &ret->lookup("oasis-bg");
	sprite_oasis_traveller = &ret->lookup("oasis-traveller");
	sprite_oasis_missing = &ret->lookup("oasis-missing");

	sprite_hill_bg = &ret->lookup("hill-bg");
	sprite_hill_traveller = &ret->lookup("hill-traveller");
	sprite_hill_missing = &ret->lookup("hill-missing");

	return ret;
});

Load< Sound::Sample > music_cold_dunes(LoadTagDefault, []() -> Sound::Sample * {
	return new Sound::Sample(data_path("cold-dunes.opus"));
});

StoryMode::StoryMode() {
}

StoryMode::~StoryMode() {
}

bool StoryMode::handle_event(SDL_Event const &, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP) {
		if (evt.key.keysym.scancode == SDL_SCANCODE_A) {
			controls.left = (evt.type == SDL_KEYDOWN);
			return true;
		} else if (evt.key.keysym.scancode == SDL_SCANCODE_D) {
			controls.right = (evt.type == SDL_KEYDOWN);
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			controls.up = (evt.type == SDL_KEYDOWN);
			return true;
		}
	}

	return false;
}

void StoryMode::update(float elapsed) {
	if (Mode::current.get() == this) {
		//there is no menu displayed! Make one:
		enter_scene(elapsed);
	}

	if (!background_music || background_music->stopped) {
		background_music = Sound::play(*music_cold_dunes, 1.0f);
	}
}

void StoryMode::enter_scene(float elapsed) {
	{
		//player motion:
		//build a shove from controls:

		glm::vec2 shove = glm::vec2(0.0f);
		if (controls.left) shove.x -= 1.0f;
		if (controls.right) shove.x += 1.0f;
		if (controls.up) shove.y += 1.0f;
		if (controls.down) shove.y -= 1.0f;
		shove *= 10.0f;

		glm::vec2 &position = player.position;
		glm::vec2 &velocity = player.velocity;
		glm::vec2 &radius = player.radius;

		velocity = glm::vec2(
			//decay existing velocity toward shove:
			glm::mix(shove, velocity, std::pow(0.5f, elapsed / 0.25f)).x,
			//also: gravity
			velocity.y - 10.0f * elapsed
		);
		position = position + velocity * elapsed;
		//---- collision handling ----
		for (int i = 0; i < parts.size(); i++) {
			for (int j = 0; j < parts[i].size(); j++) {
				glm::vec2 box = glm::vec2(parts[i][j]->position.x, parts[i][j]->position.y);
				glm::vec2 box_radius = glm::vec2(parts[i][j]->radius.x, parts[i][j]->radius.y);
				glm::vec2 min = glm::max(box - box_radius, position - radius);
				glm::vec2 max = glm::min(box + box_radius, position + radius);
				if (min.x <= max.x && min.y <= max.y) {
					switch (parts[i][j]->id())
					{
					case Part::npc_type:
						break;
					
					case Part::ground_type:
						// y direction
						if (position.y < box.y + box_radius.y + radius.y) {
							position.y = box.y + box_radius.y + radius.y;
							if (velocity.y < 0.0f) {
								velocity.y = 0.0f;
							}
						}
						else if (position.y > box.y - box_radius.y - radius.y) {
							position.y = box.y - box_radius.y - radius.y;
							if (velocity.y > 0.0f) {
								velocity.y = 0.0f;
							}
						}
						// x direction
						if (position.x < box.x + box_radius.x + radius.x) {
							position.x = box.x + box_radius.x + radius.x;
							if (velocity.x < 0.0f) {
								velocity.x = 0.0f;
							}
						}
						else if (position.x > box.x - box_radius.x - radius.x) {
							position.x = box.x - box_radius.x - radius.x;
							if (velocity.x > 0.0f) {
								velocity.x = 0.0f;
							}
						}
						break;

					case Part::empty_type:
						break;

					case Part::ingredient_type:
						// y direction
						parts[i][j]->obtained = true;
						break;

					default:
						break;
					}
				}
			}
		}
	}
	
}

void StoryMode::draw(glm::uvec2 const &drawable_size) {
	//clear the color buffer:
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//use alpha blending:
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);

	{ //use a DrawSprites to do the drawing:
		DrawSprites draw(*sprites, view_min, view_max, drawable_size, DrawSprites::AlignPixelPerfect);
		glm::vec2 ul = glm::vec2(view_min.x, view_max.y);
		if (location == Dunes) {
			draw.draw(*sprite_dunes_bg, ul);
			draw.draw(*sprite_dunes_ship, ul);
			draw.draw(*sprite_dunes_traveller, ul);
		} else if (location == Oasis) {
			draw.draw(*sprite_oasis_bg, ul);
			if (!have_stone) {
				draw.draw(*sprite_oasis_missing, ul);
			}
			draw.draw(*sprite_oasis_traveller, ul);

		} else if (location == Hill) {
			draw.draw(*sprite_hill_bg, ul);
			if (added_stone) {
				draw.draw(*sprite_hill_missing, ul);
			}
			draw.draw(*sprite_hill_traveller, ul);
		}
	}
	GL_ERRORS(); //did the DrawSprites do something wrong?
}
