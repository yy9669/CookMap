#include "MenuMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for easy sprite drawing:
#include "DrawSprites.hpp"

#include <random>

MenuMode::MenuMode(std::vector< Item > const &items_) : items(items_) {

	//select first item which can be selected:
	for (uint32_t i = 0; i < items.size(); ++i) {
		if (items[i].on_select) {
			selected = i;
			break;
		}
	}
}

MenuMode::~MenuMode() {
}

bool MenuMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_UP) {
			//skip non-selectable items:
			for (uint32_t i = selected - 1; i < items.size(); --i) {
				if (items[i].on_select) {
					selected = i;
					break;
				}
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			//note: skips non-selectable items:
			for (uint32_t i = selected + 1; i < items.size(); ++i) {
				if (items[i].on_select) {
					selected = i;
					break;
				}
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_RETURN) {
			if (selected < items.size() && items[selected].on_select) {
				items[selected].on_select(items[selected]);
				return true;
			}
		}
	}
	if (background) {
		return background->handle_event(evt, window_size);
	} else {
		return false;
	}
}

void MenuMode::update(float elapsed) {

	//TODO: selection bounce update

	if (background) {
		background->update(elapsed);
	}
}

void MenuMode::draw(glm::uvec2 const &drawable_size) {
	if (background) {
		std::shared_ptr< Mode > hold_me = shared_from_this();
		background->draw(drawable_size);
		//it is an error to remove the last reference to this object in background->draw():
		assert(hold_me.use_count() > 1);
	} else {
		glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	//extend view_min/view_max to match drawable's aspect ratio:
	glm::vec2 window_min, window_max;
	//if view_size.x / view_size.y < drawable_size.x / drawable_size.y...
	if ((view_max.x - view_min.x) * drawable_size.y < drawable_size.x * (view_max.y - view_min.y)) {
		//...need to stretch wider to match aspect:
		float w = (view_max.y - view_min.y) * float(drawable_size.x) / float(drawable_size.y);
		window_min.x = 0.5f * (view_min.x + view_max.x) - 0.5 * w;
		window_max.x = 0.5f * (view_min.x + view_max.x) + 0.5 * w;
		window_min.y = view_min.y;
		window_max.y = view_max.y;
	} else {
		//...need to stretch taller to match aspect:
		window_min.x = view_min.x;
		window_max.x = view_max.x;
		float h = (view_max.x - view_min.x) * float(drawable_size.y) / float(drawable_size.x);
		window_min.y = 0.5f * (view_min.y + view_max.y) - 0.5 * h;
		window_max.y = 0.5f * (view_min.y + view_max.y) + 0.5 * h;

	}

	//use alpha blending:
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);

	{ //draw the menu using DrawSprites:
		assert(atlas && "it is an error to try to draw a menu without an atlas");
		DrawSprites draw_sprites(*atlas, window_min, window_max, drawable_size, DrawSprites::AlignPixelPerfect);

		for (auto const &item : items) {
			if (!item.sprite) continue;
			bool is_selected = (&item == &items[0] + selected);
			draw_sprites.draw(*item.sprite, item.at, item.scale, (is_selected ? glm::u8vec4(0xff, 0x00, 0xff, 0xff) : glm::u8vec4(0xff, 0xff, 0xff, 0xff)));
			if (is_selected) {
				float left = item.at.x + item.scale * (item.sprite->min_px.x - item.sprite->anchor_px.x);
				float right = item.at.x + item.scale * (item.sprite->max_px.x - item.sprite->anchor_px.x);
				if (left_select) {
					draw_sprites.draw(*left_select, glm::vec2(left, item.at.y), item.scale);
				}
				if (right_select) {
					draw_sprites.draw(*right_select, glm::vec2(right, item.at.y), item.scale);
				}
			}
			
		}
	} //<-- gets drawn here!


	GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.
}
