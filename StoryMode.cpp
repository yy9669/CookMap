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
	if (Mode::current.get() != this) return false;

	return false;
}

void StoryMode::update(float elapsed) {
	if (Mode::current.get() == this) {
		//there is no menu displayed! Make one:
		enter_scene();
	}

	if (!background_music || background_music->stopped) {
		background_music = Sound::play(*music_cold_dunes, 1.0f);
	}
}

void StoryMode::enter_scene() {
	//just entered this scene, adjust flags and build menu as appropriate:
	std::vector< MenuMode::Item > items;
	glm::vec2 at(3.0f, view_max.y - 3.0f - 11.0f);
	auto add_text = [&items,&at](std::string text) {
		while (text.size()) {
			auto end = text.find('\n');
			items.emplace_back(text.substr(0, end), nullptr, 1.0f, glm::u8vec4(0x00, 0x00, 0x00, 0xff), nullptr, at);
			at.y -= 13.0f;
			if (end == std::string::npos) break;
			text = text.substr(end+1);
		}
		at.y -= 4.0f;
	};
	auto add_choice = [&items,&at](std::string const &text, std::function< void(MenuMode::Item const &) > const &fn) {
		items.emplace_back(text, nullptr, 1.0f, glm::u8vec4(0x00, 0x00, 0x00, 0x88), fn, at + glm::vec2(16.0f, 0.0f));
		items.back().selected_tint = glm::u8vec4(0x00, 0x00, 0x00, 0xff);
		at.y -= 13.0f;
		at.y -= 4.0f;
	};

	if (location == Dunes) {
		if (dunes.wont_leave) {
			dunes.wont_leave = false;
			add_text(
				"Something remains to accomplish.\n"
				"I won't leave."
			);
		}
		if (dunes.first_visit) {
			dunes.first_visit = false;
			add_text(
				"The landing is turbulent.\n"
				"As the sand settles, I see there is\n"
				"nobody here to meet me."
			);
		} else {
			add_text(
				"There is still nobody here to meet me."
			);
		}
		at.y -= 8.0f; //gap before choices
		add_choice("Walk West", [this](MenuMode::Item const &){
			location = Hill;
			Mode::current = shared_from_this();
		});
		add_choice("Walk East", [this](MenuMode::Item const &){
			location = Oasis;
			Mode::current = shared_from_this();
		});
		if (!dunes.first_visit) {
			add_choice("Leave", [this](MenuMode::Item const &){
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
			add_text(
				"The stone fits snugly in my pocket."
			);
		}
		if (oasis.first_visit) {
			oasis.first_visit = false;
			add_text(
				"I search east, walking in ever-\n"
				"greater circles. Just over the next\n"
				"dune, I find an oasis."
			);
		} else {
			add_text(
				"The oasis sparkles in the sunlight."
			);
		}
		if (!have_stone) {
			add_text(
				"Sitting in the glass-clear water is a\n"
				"single blue gemstone."
			);
		}
		at.y -= 8.0f; //gap before choices
		if (!have_stone) {
			add_choice("Take Stone", [this](MenuMode::Item const &){
				have_stone = true;
				oasis.took_stone = true;
				Mode::current = shared_from_this();
			});
		}
		add_choice("Return to the Ship", [this](MenuMode::Item const &){
			location = Dunes;
			Mode::current = shared_from_this();
		});
	} else if (location == Hill) {
		if (hill.added_stone) {
			hill.added_stone = false;
			add_text(
				"I add the blue stone to the circle.\n"
				"Something trembles deep underground."
			);
		}
		if (hill.first_visit) {
			hill.first_visit = false;
			add_text(
				"I set off confidently to the west.\n"
				"At the top of the third dune, a circle\n"
				"of stones surrounds a shallow\n"
				"depression in the ground."
			);
		} else {
			if (added_stone) {
				add_text(
					"The circle of stones stands silently."
				);
			} else {
				add_text(
					"The circle of stones stands\n"
					"expectantly."
				);
			}
		}
		at.y -= 8.0f; //gap before choices
		if (have_stone && !added_stone) {
			add_choice("Add Blue Stone", [this](MenuMode::Item const &){
				added_stone = true;
				hill.added_stone = true;
				Mode::current = shared_from_this();
			});
		}
		add_choice("Return to the Ship", [this](MenuMode::Item const &){
			location = Dunes;
			Mode::current = shared_from_this();
		});
	}
	std::shared_ptr< MenuMode > menu = std::make_shared< MenuMode >(items);
	menu->atlas = sprites;
	menu->left_select = sprite_left_select;
	menu->right_select = sprite_right_select;
	menu->select_bounce_amount = 4.0f;
	menu->left_select_tint = glm::u8vec4(0x00, 0x00, 0x00, 0xff);
	menu->right_select_tint = glm::u8vec4(0x00, 0x00, 0x00, 0xff);
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
