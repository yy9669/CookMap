#include "StoryMode.hpp"

#include "Sprite.hpp"
#include "DrawSprites.hpp"
#include "Load.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"
#include "MenuMode.hpp"
#include "Sound.hpp"
#include <string>
#include <fstream>

using namespace std;

Sprite const *sprite_left_select = nullptr;
Sprite const *sprite_right_select = nullptr;

Sprite const *sprite_background = nullptr;
Sprite const *sprite_chef = nullptr;
Sprite const *sprite_item_1 = nullptr;
Sprite const *sprite_item_2 = nullptr;
Sprite const *sprite_item_3 = nullptr;
Sprite const *sprite_dish_1 = nullptr;
Sprite const *sprite_npc_1 = nullptr;
Sprite const *sprite_health_box = nullptr;
Sprite const *sprite_exit = nullptr;
Sprite const *sprite_tile_1 = nullptr;


Load< SpriteAtlas > sprites(LoadTagDefault, []() -> SpriteAtlas const * {
	SpriteAtlas const *ret = new SpriteAtlas(data_path("cookmap"));

    sprite_background = &ret->lookup("background");
    sprite_chef = &ret->lookup("chef");
    sprite_item_1 = &ret->lookup("item_1");
    sprite_item_2 = &ret->lookup("item_2");
    sprite_item_3 = &ret->lookup("item_3");
    sprite_dish_1 = &ret->lookup("dish_1");
    sprite_npc_1 = &ret->lookup("guard_1");
    sprite_health_box = &ret->lookup("health_box");
    sprite_exit = &ret->lookup("exit");
    sprite_tile_1 = &ret->lookup("tile_1");

	return ret;
});

Load< Sound::Sample > music_cold_dunes(LoadTagDefault, []() -> Sound::Sample * {
	return new Sound::Sample(data_path("cold-dunes.opus"));
});

bool load_map_file(const string& filename, StoryMode* mode) {
    ifstream f(filename);

    if (!f.is_open()) {
        cerr << "Error: could NOT open file " << filename << endl;
        return false;
    }

    // reserve for space
    int width, height;
    f >> width >> height;
    if (height != 16) {
        cerr << "Error: map height must be 16" << endl;
        return false;
    }
    mode->parts.clear();
    mode->parts.resize(height);
    for (int i = 0; i < height; ++i) {
        mode->parts[i].resize(width);
    }

    // player start position
    float start_x, start_y;
    f >> start_x >> start_y;
    mode->player.set_pos(TILE_SIZE*start_x, TILE_SIZE*start_y);

    // npc definition
    int npc_num, npc_type_;
    float npc_x, npc_y;

    for (unsigned i = 0; i < mode->npcs.size(); ++i) {
        delete mode->npcs[i];
    }
    mode->npcs.clear();

    f >> npc_num;
    mode->npcs.resize(npc_num);
    for (int i = 0; i < npc_num; ++i) {
        f >> npc_type_ >> npc_x >> npc_y;
        mode->npcs[i] = new Npc((npc_type)npc_type_, TILE_SIZE*npc_x, TILE_SIZE*npc_y);
    }

    // fill in the parts map
    string s;
    for (int y = 0; y < height; ++y) {
        f >> s;
        if ((int)s.size() != width) {
            cerr << "Error: wrong map format!" << endl;
            return false;
        }
        for (int x = 0; x < width; ++x) {
            auto &part = mode->parts[y][x];
            switch (s[x]) {
                case 'e':
                    part = new Empty();
                    break;
                case 'g':
                    part = new Ground();
                    break;
                case '*':
                    part = new Goal();
                    break;
                case '1': case '2': case '3':
                    part = new Ingredient((ingredient_type)(s[x] - '1'));
                    break;
                default:
                    cerr << "Error: map type unrecognized" << endl;
                    return false;
            }
            part->set_pos(TILE_SIZE*x, TILE_SIZE*(height-1-y));
        }
    }

    f.close();
    return true;
}

StoryMode::StoryMode() {
    load_map_file(data_path("map_1.txt"), this);
}

StoryMode::~StoryMode() {
}

bool StoryMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
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
    if (evt.type== SDL_MOUSEBUTTONDOWN){
        if(evt.button.x>= 866 && 
        evt.button.x<=1005 && evt.button.y>=5 && evt.button.y<=69) {
            proto_cook=true;
        }else if (evt.button.x>= 740 && 
        evt.button.x<=843 && (evt.button.x-740)%55<48 && evt.button.y>=10 && evt.button.y<=58 && (evt.button.x-740)/55 <dishes.size()  ) {
                            dishes.erase(dishes.begin()+(evt.button.x-740)/55 );
                            dish_drag=true;
        }
                    return true;
    }
    if(evt.type== SDL_MOUSEMOTION && dish_drag==true){
        dish_drag_pos=glm::vec2(evt.x,evt.y)
        return true;
    }
    if (evt.type== SDL_MOUSEBUTTONUP && dish_drag==true){
        dish_drag==false;
        
        return true;
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

bool StoryMode::collision(glm::vec2 pos1, glm::vec2 radius1, glm::vec2 pos2, 
    glm::vec2 radius2) {
    glm::vec2 posmin = glm::vec2(max(pos1.x, pos2.x), max(pos1.y, pos2.y));
    glm::vec2 posmax = glm::vec2(min(pos1.x+radius1.x, pos2.x+radius2.x),
                                    min(pos1.y+radius1.y, pos2.y+radius2.y));
    return posmin.x < posmax.x && posmin.y < posmax.y;
}

void StoryMode::resolve_collision(glm::vec2 &position, glm::vec2 radius, 
    glm::vec2 box, glm::vec2 box_radius, glm::vec2 &velocity) {
    // y direction
    if (position.y < box.y + box_radius.y && 
        position.y > box.y + box_radius.y - 5.0f &&
        box.y + box_radius.y <= position.y + radius.y) {
        position.y = box.y + box_radius.y;
        if (velocity.y < 0.0f) {
            velocity.y = 0.0f;
        }
    }
    else if (position.y > box.y - radius.y && 
        position.y < box.y - radius.y + 5.0f &&
        box.y >= position.y) {
        position.y = box.y - radius.y;
        if (velocity.y > 0.0f) {
            velocity.y = 0.0f;
        }
    }
    // x direction
    else if (position.x < box.x + box_radius.x &&
        box.x + box_radius.x <= position.x + radius.x) {
        position.x = box.x + box_radius.x;
        if (velocity.x < 0.0f) {
            velocity.x = 0.0f;
        }
    }
    else if (position.x > box.x - radius.x &&
        box.x >= position.x) {
        position.x = box.x - radius.x;
        if (velocity.x > 0.0f) {
            velocity.x = 0.0f;
        }
    }
}

void StoryMode::enter_scene(float elapsed) {
	{
		//player motion:
		//build a shove from controls:

        glm::vec2 &position = player.position;
        glm::vec2 &velocity = player.velocity;
        glm::vec2 &radius = player.radius;

		glm::vec2 shove = glm::vec2(0.0f);
		if (controls.left) shove.x -= 15.0f;
		if (controls.right) shove.x += 15.0f;
		if (controls.up && abs(velocity.y) < 1e-4) {
		    shove.y += 20.0f;
            controls.up = false;
		}
		shove *= 10.0f;

		auto tmp = glm::mix(shove, velocity, std::pow(0.5f, elapsed / 0.25f));
		velocity = glm::vec2(tmp.x, velocity.y + shove.y - 200.0f * elapsed);

        position = position + velocity * elapsed;

		//---- collision handling ----
		for (unsigned i = 0; i < parts.size(); i++) {
			for (unsigned j = 0; j < parts[i].size(); j++) {
                glm::vec2 box = parts[i][j]->position;
                glm::vec2 box_radius = parts[i][j]->radius;
				if (collision(position, radius, box, box_radius)) {
                    Ingredient *ingre;
					switch (parts[i][j]->id())
					{
					case part_ground_type:
						resolve_collision(position, radius, box, box_radius, velocity);
						break;

					case part_empty_type:
						break;

					case part_ingredient_type:
                        ingre = reinterpret_cast<Ingredient*>(parts[i][j]);
                        if (!ingre->obtained && backpack.size() < 5) {
                            ingre->obtained = true;
                            backpack.push_back(ingre->type);
                        }
						break;

                    case part_goal_type:
                        break;
					default:
						break;
					}
				}
			}
		}
        // Npc detection
        for (unsigned i = 0; i < npcs.size(); i++) {
            glm::vec2 box = npcs[i]->position;
            glm::vec2 box_radius = npcs[i]->radius;
            if (collision(position, radius, box, box_radius)) {
                resolve_collision(position, radius, box, box_radius, velocity);
            }
        }

        if (position.x + radius.x > parts[0].size() * TILE_SIZE) {
            position.x = parts[0].size() * TILE_SIZE - radius.x;
            velocity.x = 0.0f;
        }
        if (position.x < 0) {
            position.x = 0;
            velocity.x = 0.0f;
        }
	}
	{
        // slide viewport window
        float left_border = max(player.position.x - 512.0f, 0.0f);
        left_border = min(left_border, parts[0].size() * TILE_SIZE - 1024);

        view_min = glm::vec2(left_border, 0);
        view_max = glm::vec2(left_border + 1024.0f, view_max.y);
    }
            if(proto_cook){
                proto_cook = false;
                if(dishes.size() < 2 && backpack.size() > 0){
                    backpack.clear();
                    dishes.push_back(Dish1);
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
		DrawSprites draw(*sprites, view_min, view_max, drawable_size, DrawSprites::AlignSloppy);
		glm::vec2 bl = glm::vec2(view_min.x, view_min.y);

		if (game_mode == Walking) {
            draw.draw(*sprite_background, bl);

            for (unsigned i = 0; i < parts.size(); i++) {
                for (unsigned j = 0; j < parts[i].size(); j++) {
                    Ingredient *ingre;
                    switch (parts[i][j]->id())
                    {
                        case part_ground_type:
                            draw.draw(*sprite_tile_1, parts[i][j]->position);
                            break;

                        case part_empty_type:
                            break;

                        case part_ingredient_type:
                            ingre = reinterpret_cast<Ingredient*>(parts[i][j]);
                            if (!ingre->obtained) {
                                switch (ingre->type) {
                                    case Tomato:
                                        draw.draw(*sprite_item_1, parts[i][j]->position);
                                        break;
                                    case Potato:
                                        draw.draw(*sprite_item_2, parts[i][j]->position);
                                        break;
                                    case Onion:
                                        draw.draw(*sprite_item_3, parts[i][j]->position);
                                        break;
                                }
                            }
                            break;

                        case part_goal_type:
                            draw.draw(*sprite_exit, parts[i][j]->position);
                            break;

                        default:
                            break;
                    }
                }
            }

            for (unsigned i = 0; i < npcs.size(); i++) {
                switch (npcs[i]->type)
                {
                    case npc0:
                        draw.draw(*sprite_npc_1, npcs[i]->position);
                        break;
                    case npc1:
                        draw.draw(*sprite_npc_1, npcs[i]->position);
                        break;
                    case npc2:
                        draw.draw(*sprite_npc_1, npcs[i]->position);
                        break;
                    case npc3:
                        draw.draw(*sprite_npc_1, npcs[i]->position);
                        break;
                    default:
                        break;
                }
            }

            for (unsigned i = 0; i < backpack.size(); i++) {
                switch (backpack[i]) {
                    case Tomato:
                        draw.draw(*sprite_item_1, backpack_pos[i]+view_min);
                        break;
                    case Potato:
                        draw.draw(*sprite_item_2, backpack_pos[i]+view_min);
                        break;
                    case Onion:
                        draw.draw(*sprite_item_3, backpack_pos[i]+view_min);
                        break;
                }
            }

            for (unsigned i = 0; i < dishes.size(); i++) {
                //TODO: replace item with dish
                switch (dishes[i]) {
                    case Dish1:
                        draw.draw(*sprite_dish_1, dishes_pos[i]+view_min);
                        break;
                    case Dish2:
                        draw.draw(*sprite_dish_1, dishes_pos[i]+view_min);
                        break;
                    case Dish3:
                        draw.draw(*sprite_dish_1, dishes_pos[i]+view_min);
                        break;
                }
            }

            if(dish_drag){
                draw.draw(*sprite_dish_1, dish_drag_pos)
            }

            draw.draw(*sprite_chef, player.position);
            glm::vec2 health_pos = glm::vec2(50.0f, 726.0f);
            for (int h = 0; h < player.health; h++) {
                draw.draw(*sprite_health_box, health_pos+view_min);
                health_pos.x += 20.0f;
            }

		} else {
            // cooking
		}

	}
	GL_ERRORS(); //did the DrawSprites do something wrong?
}
