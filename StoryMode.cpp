#include "StoryMode.hpp"

#include "Sprite.hpp"
#include "DrawSprites.hpp"
#include "Load.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"
#include "MenuMode.hpp"

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

Sprite const *text_dunes_landing = nullptr;
Sprite const *text_dunes_return = nullptr;
Sprite const *text_dunes_wont_leave = nullptr;
Sprite const *text_dunes_do_leave = nullptr;
Sprite const *text_dunes_do_walk_east = nullptr;
Sprite const *text_dunes_do_walk_west = nullptr;

Sprite const *text_oasis_intro = nullptr;
Sprite const *text_oasis_stone = nullptr;
Sprite const *text_oasis_plain = nullptr;
Sprite const *text_oasis_stone_taken = nullptr;
Sprite const *text_oasis_do_take_stone = nullptr;
Sprite const *text_oasis_do_return = nullptr;

Sprite const *text_hill_intro = nullptr;
Sprite const *text_hill_inactive = nullptr;
Sprite const *text_hill_active = nullptr;
Sprite const *text_hill_stone_added = nullptr;
Sprite const *text_hill_do_add_stone = nullptr;
Sprite const *text_hill_do_return = nullptr;

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

	text_dunes_landing = &ret->lookup("dunes-text-landing");
	text_dunes_return = &ret->lookup("dunes-text-return");
	text_dunes_wont_leave = &ret->lookup("dunes-text-won't-leave");
	text_dunes_do_leave = &ret->lookup("dunes-text-do-leave");
	text_dunes_do_walk_east = &ret->lookup("dunes-text-do-walk-east");
	text_dunes_do_walk_west = &ret->lookup("dunes-text-do-walk-west");

	text_oasis_intro = &ret->lookup("oasis-text-intro");
	text_oasis_stone = &ret->lookup("oasis-text-stone");
	text_oasis_plain = &ret->lookup("oasis-text-plain");
	text_oasis_stone_taken = &ret->lookup("oasis-text-stone-taken");
	text_oasis_do_take_stone = &ret->lookup("oasis-text-do-take-stone");
	text_oasis_do_return = &ret->lookup("oasis-text-do-return");

	text_hill_intro = &ret->lookup("hill-text-intro");
	text_hill_inactive = &ret->lookup("hill-text-inactive");
	text_hill_active = &ret->lookup("hill-text-active");
	text_hill_stone_added = &ret->lookup("hill-text-stone-added");
	text_hill_do_add_stone = &ret->lookup("hill-text-do-add-stone");
	text_hill_do_return = &ret->lookup("hill-text-do-return");

	return ret;
});

StoryMode::StoryMode() {
}

StoryMode::~StoryMode() {
}

bool StoryMode::handle_event(SDL_Event const &, glm::uvec2 const &window_size) {
	if (Mode::current.get() != this) return false;

	return false;
}

void StoryMode::update(float elapsed) {
	if (Mode::current.get() == this) {
		//there is no menu displayed! Make one:
		enter_scene();
	}
}

void StoryMode::enter_scene() {
	//just entered this scene, adjust flags and build menu as appropriate:
	std::vector< MenuMode::Item > items;
	glm::vec2 at(3.0f, view_max.y - 3.0f);
	auto add_text = [&items,&at](Sprite const *text) {
		assert(text);
		items.emplace_back("TEST TEXT", nullptr, 1.0f, nullptr, at);
		at.y -= text->max_px.y - text->min_px.y;
		at.y -= 4.0f;
	};
	auto add_choice = [&items,&at](Sprite const *text, std::function< void(MenuMode::Item const &) > const &fn) {
		assert(text);
		items.emplace_back("TEST CHOICE", nullptr, 1.0f, fn, at + glm::vec2(8.0f, 0.0f));
		at.y -= text->max_px.y - text->min_px.y;
		at.y -= 4.0f;
	};

	if (location == Dunes) {
		if (dunes.wont_leave) {
			dunes.wont_leave = false;
			add_text(text_dunes_wont_leave);
		}
		if (dunes.first_visit) {
			dunes.first_visit = false;
			add_text(text_dunes_landing);
		} else {
			add_text(text_dunes_return);
		}
		at.y -= 8.0f; //gap before choices
		add_choice(text_dunes_do_walk_west, [this](MenuMode::Item const &){
			location = Hill;
			Mode::current = shared_from_this();
		});
		add_choice(text_dunes_do_walk_east, [this](MenuMode::Item const &){
			location = Oasis;
			Mode::current = shared_from_this();
		});
		if (!dunes.first_visit) {
			add_choice(text_dunes_do_leave, [this](MenuMode::Item const &){
				if (added_stone) {
					//TODO: some sort of victory animation?
					Mode::current = nullptr;
				} else {
					dunes.wont_leave = true;
					Mode::current = shared_from_this();
				}
			});
		}
	} else if (location == Oasis) {
		if (oasis.took_stone) {
			oasis.took_stone = false;
			add_text(text_oasis_stone_taken);
		}
		if (oasis.first_visit) {
			oasis.first_visit = false;
			add_text(text_oasis_intro);
		} else {
			add_text(text_oasis_plain);
		}
		if (!have_stone) {
			add_text(text_oasis_stone);
		}
		at.y -= 8.0f; //gap before choices
		if (!have_stone) {
			add_choice(text_oasis_do_take_stone, [this](MenuMode::Item const &){
				have_stone = true;
				oasis.took_stone = true;
				Mode::current = shared_from_this();
			});
		}
		add_choice(text_oasis_do_return, [this](MenuMode::Item const &){
			location = Dunes;
			Mode::current = shared_from_this();
		});
	} else if (location == Hill) {
		if (hill.added_stone) {
			hill.added_stone = false;
			add_text(text_hill_stone_added);
		}
		if (hill.first_visit) {
			hill.first_visit = false;
			add_text(text_hill_intro);
		} else {
			if (added_stone) {
				add_text(text_hill_active);
			} else {
				add_text(text_hill_inactive);
			}
		}
		at.y -= 8.0f; //gap before choices
		if (have_stone && !added_stone) {
			add_choice(text_hill_do_add_stone, [this](MenuMode::Item const &){
				added_stone = true;
				hill.added_stone = true;
				Mode::current = shared_from_this();
			});
		}
		add_choice(text_hill_do_return, [this](MenuMode::Item const &){
			location = Dunes;
			Mode::current = shared_from_this();
		});
	}
	std::shared_ptr< MenuMode > menu = std::make_shared< MenuMode >(items);
	menu->atlas = sprites;
	menu->left_select = sprite_left_select;
	menu->right_select = sprite_right_select;
	menu->view_min = view_min;
	menu->view_max = view_max;
	menu->background = shared_from_this();
	Mode::current = menu;
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
