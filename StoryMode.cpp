#include "StoryMode.hpp"

#include "Sprite.hpp"
#include "Load.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"
#include "MenuMode.hpp"
#include "Sound.hpp"
#include <string>
#include <fstream>
#include <map>

using namespace std;

Sprite const *sprite_left_select = nullptr;
Sprite const *sprite_right_select = nullptr;
Sprite const *sprite_background = nullptr;

Sprite const *sprite_chef_left_stand = nullptr;
Sprite const *sprite_chef_left_walk1 = nullptr;
Sprite const *sprite_chef_left_walk2 = nullptr;
Sprite const *sprite_chef_left_jump = nullptr;
Sprite const *sprite_chef_right_stand = nullptr;
Sprite const *sprite_chef_right_walk1 = nullptr;
Sprite const *sprite_chef_right_walk2 = nullptr;
Sprite const *sprite_chef_right_jump = nullptr;

Sprite const *sprite_item_1 = nullptr;
Sprite const *sprite_item_2 = nullptr;
Sprite const *sprite_item_3 = nullptr;
Sprite const *sprite_item_question = nullptr;

Sprite const *sprite_pizza = nullptr;
Sprite const *sprite_dish_2 = nullptr;
Sprite const *sprite_dish_3 = nullptr;

Sprite const *sprite_npc_1 = nullptr;
Sprite const *sprite_npc_1_idle = nullptr;

Sprite const *sprite_health_box = nullptr;
Sprite const *sprite_exit = nullptr;
Sprite const *sprite_tile_1 = nullptr;
Sprite const *sprite_tile_2 = nullptr;
Sprite const *sprite_instruction_panel = nullptr;
Sprite const *sprite_helper = nullptr;
Sprite const *sprite_recipe = nullptr;
Sprite const *sprite_pot = nullptr;
Sprite const *sprite_fire = nullptr;

typedef struct {
    vector<ingredient_type> ingredients;
    vector<bool> show;
    dish_type dish;
    int restore;
} Recipe;

vector<Recipe> recipes = {
    {{Tomato, Potato, Onion}, {true, true, true}, Pizza, 10},
    {{Tomato, Onion}, {true, true}, Pizza, 10},
};

Load< SpriteAtlas > sprites(LoadTagDefault, []() -> SpriteAtlas const * {
	SpriteAtlas const *ret = new SpriteAtlas(data_path("cookmap"));

    sprite_background = &ret->lookup("background");
    sprite_chef_left_stand = &ret->lookup("chef");
    sprite_chef_left_walk1 = &ret->lookup("chef");
    sprite_chef_left_walk2 = &ret->lookup("chef");
    sprite_chef_left_jump = &ret->lookup("chef");
    sprite_chef_right_stand = &ret->lookup("chef");
    sprite_chef_right_walk1 = &ret->lookup("chef");
    sprite_chef_right_walk2 = &ret->lookup("chef");
    sprite_chef_right_jump = &ret->lookup("chef");

    sprite_item_1 = &ret->lookup("item_1");
    sprite_item_2 = &ret->lookup("item_2");
    sprite_item_3 = &ret->lookup("item_3");
    sprite_pizza = &ret->lookup("dish_1");
    sprite_dish_2 = &ret->lookup("dish_1");  // to be changed
    sprite_dish_3 = &ret->lookup("dish_1");  // to be changed
    sprite_npc_1 = &ret->lookup("guard_1");
    sprite_npc_1_idle = &ret->lookup("guard_1_idle");
    sprite_health_box = &ret->lookup("health_box");
    sprite_exit = &ret->lookup("exit");
    sprite_tile_1 = &ret->lookup("tile_1");
    sprite_tile_2 = &ret->lookup("tile_2");
    sprite_instruction_panel = &ret->lookup("panel_1");
    sprite_helper = &ret->lookup("help");
    sprite_recipe = &ret->lookup("help");  // to be changed
    sprite_pot = &ret->lookup("help");  // to be changed
    sprite_fire = &ret->lookup("help");  // to be changed
    sprite_item_question = &ret->lookup("item_1");  // to be changed
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
                case 'g': case 'w':
                    part = new Ground(s[x]);
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
    if (winning || lose) {
        return false;
    }
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
//        if (evt.button.x>= 866 && evt.button.x<=1005 &&
//            evt.button.y>=5 && evt.button.y<=69) {
//            proto_cook=true;
//        } else
        if (evt.button.x>= 740 && evt.button.x<=843 &&
           (evt.button.x-740)%55<48 && evt.button.y>=10 && evt.button.y<=58 &&
           int((evt.button.x-740)/55) <int(dishes.size() ) ) {
            dishes.erase(dishes.begin() + (evt.button.x-740)/55 );
            dish_drag = true;
            dish_drag_pos = glm::vec2(evt.button.x,768-evt.button.y)+view_min;
        } else if (evt.button.x>= 360 && evt.button.x<=628 &&
                   (evt.button.x-360)%55<48 && evt.button.y>=10 && evt.button.y<=58 &&
                   int((evt.button.x-360)/55) <int(backpack.size() ) ) {
            // drag from backpack
            dragging_ingre_type = backpack[(evt.button.x-360)/55];
            backpack.erase(backpack.begin() + (evt.button.x-360)/55 );
            ingre_drag = true;
            drag_from_backpack = true;
            ingre_drag_pos = glm::vec2(evt.button.x,768-evt.button.y)+view_min;
        } else if (evt.button.x>= 965 && evt.button.x<=1013 &&
                   (evt.button.y-77)%60<48 && evt.button.y>=77 && evt.button.y<=245 &&
                   int((evt.button.y-77)/60) <int(pots.size() ) ) {
            // drag from pot
            dragging_ingre_type = pots[(evt.button.y-77)/60];
            pots.erase(pots.begin() + (evt.button.y-77)/60 );
            ingre_drag = true;
            drag_from_backpack = false;
            ingre_drag_pos = glm::vec2(evt.button.x,768-evt.button.y)+view_min;
        } else if (evt.button.x>= 0 && evt.button.x<=50 &&
                   evt.button.y>=50 && evt.button.y<=100) {
            show_instruction =! show_instruction;
        } else if (evt.button.x>= 904 && evt.button.x<=952 &&
                   evt.button.y>=10 && evt.button.y<=58) {
            show_recipe = !show_recipe;
        } else if (evt.button.x>= 964 && evt.button.x<=1012 &&
                   evt.button.y>=10 && evt.button.y<=58) {
            show_pot = !show_pot;
        } else if (evt.button.x>= 965 && evt.button.x<=1013 &&
                   evt.button.y>=263 && evt.button.y<=311 &&
                   show_pot && pot_time_left == 0.f && pots.size()) {
            pot_time_left = 5.f;
        }
        return true;
    }
    if(evt.type== SDL_MOUSEMOTION && dish_drag){
        dish_drag_pos=glm::vec2(evt.motion.x,768-evt.motion.y)+view_min;
        return true;
    }
    if (evt.type== SDL_MOUSEBUTTONUP && dish_drag){
        dish_drag=false;
        if(collision(dish_drag_pos, glm::vec2(48,48), npcs[0]->position, npcs[0]->radius )){
            npcs[0]->eat=true;
        }else{
            dishes.push_back(Pizza);
        }
        return true;
    }

    if(evt.type== SDL_MOUSEMOTION && ingre_drag){
        ingre_drag_pos=glm::vec2(evt.motion.x,768-evt.motion.y)+view_min;
        return true;
    }
    if (evt.type== SDL_MOUSEBUTTONUP && ingre_drag){
        ingre_drag=false;
        if (ingre_drag_pos.x >= 950.f+view_min.x && ingre_drag_pos.x <= 1013.f+view_min.x  &&
            ingre_drag_pos.y >= 513.f+view_min.y && ingre_drag_pos.y <= 690.f+view_min.y && show_pot && pots.size() < 3) {
            // drag to pot
            pots.push_back(dragging_ingre_type);
        } else if (evt.button.x>= 360 && evt.button.x<=628 &&
                   (evt.button.x-360)%55<48 && evt.button.y>=10 && evt.button.y<=58 &&
                   int((evt.button.x-360)/55) >= int(backpack.size() ) ) {
            // drag to backpack
            backpack.push_back(dragging_ingre_type);
        } else{
            if (drag_from_backpack) {
                backpack.push_back(dragging_ingre_type);
            } else {
                pots.push_back(dragging_ingre_type);
            }
        }
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
        //Npc motion:
        for (unsigned i = 0; i < npcs.size(); i++) {
            glm::vec2 &position_i = npcs[i]->position;
            glm::vec2 &init_position_i = npcs[i]->init_position;
            glm::vec2 &velocity_i = npcs[i]->velocity;
            switch (npcs[i]->type)
            {
                //TODO: add different motions for npc
                case npc0:
                case npc1:
                case npc2:
                case npc3:
                default:
                    if (npcs[i]->eat) {
                        position_i = position_i + velocity_i * elapsed;
                        if (position_i.y <= init_position_i.y) {
                            position_i.y = init_position_i.y;
                            velocity_i.y = -velocity_i.y;
                        } else if (position_i.y > init_position_i.y 
                            && position_i.y - init_position_i.y > 240.0f) {
                            velocity_i.y = -velocity_i.y;
                        }
                    }
                    break;
            }
        }

		//player motion:
		//build a shove from controls:
        glm::vec2 &position = player.position;
        glm::vec2 &velocity = player.velocity;
        glm::vec2 &radius = player.radius;

		glm::vec2 shove = glm::vec2(0.0f);
		if (controls.left) shove.x -= 20.0f;
		if (controls.right) shove.x += 20.0f;
		if (controls.up && abs(velocity.y) < 1e-4) {
		    shove.y += 38.5f;
            controls.up = false;
		}
		shove *= 10.0f;

		auto tmp = glm::mix(shove, velocity, std::pow(0.5f, elapsed / 0.25f));
		velocity = glm::vec2(tmp.x, velocity.y + shove.y - 720.0f * elapsed);
        // bound the negative velocity to avoid collision error
        if (velocity.y < -250.0f) {
            velocity = glm::vec2(velocity.x, -250.0f);
        }

        cout << velocity.x << "  " << velocity.y << endl;
        // change player state to draw animations
        if (velocity.y > 0.0f || velocity.y <= -13.0f) {
            if (velocity.x < 0.0f || player.state == Left_stand) {
                player.state = Left_jump;
            } else {
                player.state = Right_jump;
            }
        } else {
            if (velocity.x == 0.0f) {
                if (player.state == Left_jump || player.state == Left_stand 
                    || player.state == Left_walk1 || player.state == Left_walk2) {
                    player.state = Left_stand;
                } else {
                    player.state = Right_stand;
                }
            } else if (velocity.x > 0.0f && velocity.x < 20.0f) {
                player.state = Right_stand;
            } else if (velocity.x >= 10.0f) {
                if (player.state == Right_walk1)
                    player.state = Right_walk2;
                else
                    player.state = Right_walk1;
            } else if (velocity.x < 0.0f && velocity.x > -20.0f) {
                player.state = Left_stand;
            } else {
                if (player.state == Left_walk1)
                    player.state = Left_walk2;
                else
                    player.state = Left_walk1;
            }
        }
        cout << player.state << endl;

        position = position + velocity * elapsed;

        //---- collision handling ----
        // Npc detection
        for (unsigned i = 0; i < npcs.size(); i++) {
            glm::vec2 box = npcs[i]->position;
            glm::vec2 box_radius = npcs[i]->radius;
            if(npcs[i]->eat)
                continue;
            if (collision(position, radius, box, box_radius)) {
                resolve_collision(position, radius, box, box_radius, velocity);
                player.health -= 1;
                player.health = max(0,player.health);
                if (position.x <= npcs[i]->init_position.x)
                    velocity.x = -200.0f;
                else
                    velocity.x = 200.0f;
            }
        }

		// Environment detection
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

					case part_ingredient_type:
                        ingre = reinterpret_cast<Ingredient*>(parts[i][j]);
                        if (!ingre->obtained && backpack.size() < 5) {
                            ingre->obtained = true;
                            backpack.push_back(ingre->type);
                        }
						break;

                    case part_goal_type:
                        if (!lose) {
                            winning = true;
                        }
                        break;

					default:
						break;
					}
				}
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

        view_min = glm::vec2(int(left_border), 0);
        view_max = glm::vec2(int(left_border) + 1024, view_max.y);
    }
    if (proto_cook) {
        proto_cook = false;
        if (dishes.size() < 2 && backpack.size() > 0) {
            backpack.clear();
            dishes.push_back(Pizza);
        }
    }

    if (pot_time_left > 0.f) {
        pot_time_left -= elapsed;
        if (pot_time_left <= 0.f) {
            // check making dish
            pot_time_left = 0.f;
            unordered_map<ingredient_type, int> num;
            for (auto i : pots) {
                num[i]++;
            }
            for (auto &recipe : recipes) {
                auto num2 = num;
                bool ok = true;
                for (auto i : recipe.ingredients) {
                    if (num2.count(i) == 0 || num2[i] == 0) {
                        ok = false;
                        break;
                    }
                    num2[i]--;
                }
                if (ok) {
                    dishes.push_back(recipe.dish);
                    break;
                }
            }
            pots.clear();
        }
    }
    if (!winning && player.health == 0) {
        lose = true;
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
                    Ground *grd;
                    switch (parts[i][j]->id())
                    {
                        case part_ground_type:
                            grd = reinterpret_cast<Ground*>(parts[i][j]);
                            switch (grd->g_type) {
                            case 'g':
                                draw.draw(*sprite_tile_1, parts[i][j]->position);
                                break;
                            case 'w':
                                draw.draw(*sprite_tile_2, parts[i][j]->position);
                                break;
                            }
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
                        if (npcs[i]->eat)
                            draw.draw(*sprite_npc_1_idle, npcs[i]->position);
                        else
                            draw.draw(*sprite_npc_1, npcs[i]->position);
                        break;
                    case npc1:
                        if (npcs[i]->eat)
                            draw.draw(*sprite_npc_1_idle, npcs[i]->position);
                        else
                            draw.draw(*sprite_npc_1, npcs[i]->position);
                        break;
                    case npc2:
                        if (npcs[i]->eat)
                            draw.draw(*sprite_npc_1_idle, npcs[i]->position);
                        else
                            draw.draw(*sprite_npc_1, npcs[i]->position);
                        break;
                    case npc3:
                        if (npcs[i]->eat)
                            draw.draw(*sprite_npc_1_idle, npcs[i]->position);
                        else
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
                    case Pizza:
                        draw.draw(*sprite_pizza, dishes_pos[i]+view_min);
                        break;
                    case Dish2:
                        draw.draw(*sprite_pizza, dishes_pos[i]+view_min);
                        break;
                    case Dish3:
                        draw.draw(*sprite_pizza, dishes_pos[i]+view_min);
                        break;
                }
            }

            switch (player.state) {
                case Left_stand:
                    draw.draw(*sprite_chef_left_stand, player.position);
                    break;
                case Left_walk1:
                    draw.draw(*sprite_chef_left_walk1, player.position);
                    break;
                case Left_walk2:
                    draw.draw(*sprite_chef_left_walk2, player.position);
                    break;
                case Left_jump:
                    draw.draw(*sprite_chef_left_jump, player.position);
                    break;
                case Right_stand:
                    draw.draw(*sprite_chef_right_stand, player.position);
                    break;
                case Right_walk1:
                    draw.draw(*sprite_chef_right_walk1, player.position);
                    break;
                case Right_walk2:
                    draw.draw(*sprite_chef_right_walk2, player.position);
                    break;
                case Right_jump:
                    draw.draw(*sprite_chef_right_jump, player.position);
                    break;
            }
            
            draw.draw(*sprite_helper, glm::vec2(0.0f, 668.0f)+view_min);

            draw.draw(*sprite_recipe, glm::vec2(904.0f, 710.0f)+view_min);
            draw.draw(*sprite_pot, glm::vec2(964.0f, 710.0f)+view_min);

            glm::vec2 health_pos = glm::vec2(50.0f, 726.0f);
            for (int h = 0; h < player.health; h++) {
                draw.draw(*sprite_health_box, health_pos+view_min);
                health_pos.x += 20.0f;
            }

            if (winning) {
                // 'I' is too thin...
                draw.draw_text("YOU  W I N!", glm::vec2(160.0f, 330.0f)+view_min, 0.4);
            }
            if (lose) {
                draw.draw_text("YOU   LOSE...", glm::vec2(90.0f, 330.0f)+view_min, 0.4);
            }

            if (show_instruction) {
                draw_instruction(draw);
            }

            if (show_recipe) {
                draw_recipe(draw);
            }

            if (show_pot) {
                draw_pot(draw);
            }

            if(dish_drag){
                draw.draw(*sprite_pizza, dish_drag_pos);
            }

            if(ingre_drag){
                switch (dragging_ingre_type) {
                    case Tomato:
                        draw.draw(*sprite_item_1, ingre_drag_pos);
                        break;
                    case Potato:
                        draw.draw(*sprite_item_2, ingre_drag_pos);
                        break;
                    case Onion:
                        draw.draw(*sprite_item_3, ingre_drag_pos);
                        break;
                }
            }
		} else {
            // cooking
		}

	}
	GL_ERRORS(); //did the DrawSprites do something wrong?
}


void StoryMode::draw_instruction(DrawSprites& draw) {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 3; ++j) {
            draw.draw(*sprite_instruction_panel,
                    glm::vec2(7+48.f*i, 615-48.f*j)+view_min);
        }
    }
    draw.draw_text(
            "a,d   to   move   left,right\n"
            "space   to   jump\n"
            "click   COOK!   to   make   dish\n"
            "drag   dish   to   enemy   to   bribe",
            glm::vec2(15, 630)+view_min, 0.068);
}

void StoryMode::draw_pot(DrawSprites& draw) {
    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 5; ++j) {
            draw.draw(*sprite_instruction_panel,
                      glm::vec2(965+48.f*i, 645-48.f*j)+view_min);
        }
    }
    draw.draw(*sprite_tile_1, glm::vec2(965.f, 510.f)+view_min, glm::vec2(1.f, 0.05f), glm::u8vec4(0x00, 0x00, 0x00, 0xff));
    if (pot_time_left > 0.f) {
        string tmp = "0";
        tmp[0] = '0' + int(pot_time_left+0.999f);
        draw.draw_text(tmp, glm::vec2(980.f, 461.f)+view_min, 0.07);
    } else {
        draw.draw(*sprite_fire, glm::vec2(965.f, 457.f)+view_min);
    }

    for (unsigned i = 0; i < pots.size(); ++i) {
        auto pos = glm::vec2(965.f, 643-60.f*i)+view_min;
        switch (pots[i]) {
            case Tomato:
                draw.draw(*sprite_item_1, pos);
                break;
            case Potato:
                draw.draw(*sprite_item_2, pos);
                break;
            case Onion:
                draw.draw(*sprite_item_3, pos);
                break;
        }
    }
}

void StoryMode::draw_recipe(DrawSprites& draw) {
    for (int i = 0; i < 6; ++i) {
        for (unsigned j = 0; j < 1 + recipes.size(); ++j) {
            draw.draw(*sprite_instruction_panel,
                      glm::vec2(665+48.f*i, 645-48.f*j)+view_min);
        }
    }

    for (unsigned i = 0; i < recipes.size(); ++i) {
        for (unsigned j = 0; j < recipes[i].ingredients.size(); ++j) {
            auto pos = glm::vec2(677+60.f*j, 631-60.f*i)+view_min;
            if (recipes[i].show[j]) {
                switch (recipes[i].ingredients[j]) {
                    case Tomato:
                        draw.draw(*sprite_item_1, pos);
                        break;
                    case Potato:
                        draw.draw(*sprite_item_2, pos);
                        break;
                    case Onion:
                        draw.draw(*sprite_item_3, pos);
                        break;
                }
            } else {
                draw.draw(*sprite_item_question, pos);
            }
        }
        auto pos = glm::vec2(865.f, 631-60.f*i)+view_min;
        draw.draw(*sprite_tile_1, pos, glm::vec2(0.05f, 1.0f), glm::u8vec4(0x00, 0x00, 0x00, 0xff));
        pos = glm::vec2(885.f, 631-60.f*i)+view_min;
        switch (recipes[i].dish) {
            case Pizza:
                draw.draw(*sprite_pizza, pos);
                break;
            case Dish2:
                draw.draw(*sprite_dish_2, pos);
                break;
            case Dish3:
                draw.draw(*sprite_dish_3, pos);
                break;
        }
    }
}
