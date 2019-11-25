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
#include <algorithm>

using namespace std;

/////////////////////////////////////////////////////////////// sprite ///////////////////////////////////////////////////
const int SCENE_TOTAL = 3;
const int TILE_TOTAL = 8;

Sprite const *sprite_left_select = nullptr;
Sprite const *sprite_right_select = nullptr;
Sprite const *sprite_background[SCENE_TOTAL];

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
Sprite const *sprite_item_4 = nullptr;
Sprite const *sprite_item_5 = nullptr;
Sprite const *sprite_item_6 = nullptr;
Sprite const *sprite_item_7 = nullptr;
Sprite const *sprite_item_8 = nullptr;
Sprite const *sprite_item_9 = nullptr;
Sprite const *sprite_item_10 = nullptr;
Sprite const *sprite_item_11 = nullptr;
Sprite const *sprite_item_12 = nullptr;
Sprite const *sprite_item_13 = nullptr;
Sprite const *sprite_item_14 = nullptr;
Sprite const *sprite_item_15 = nullptr;
Sprite const *sprite_item_16 = nullptr;
Sprite const *sprite_item_17 = nullptr;
Sprite const *sprite_item_18 = nullptr;
Sprite const *sprite_item_19 = nullptr;
Sprite const *sprite_item_20 = nullptr;
Sprite const *sprite_item_21 = nullptr;
Sprite const *sprite_item_22 = nullptr;
Sprite const *sprite_item_question = nullptr;

Sprite const *sprite_dish_0 = nullptr;
Sprite const *sprite_dish_1 = nullptr;
Sprite const *sprite_dish_2 = nullptr;
Sprite const *sprite_dish_3 = nullptr;
Sprite const *sprite_dish_4 = nullptr;
Sprite const *sprite_dish_5 = nullptr;
Sprite const *sprite_dish_6 = nullptr;
Sprite const *sprite_dish_7 = nullptr;
Sprite const *sprite_dish_8 = nullptr;
Sprite const *sprite_dish_9 = nullptr;


Sprite const *sprite_npc[npc_total];
Sprite const *sprite_npc_idle[npc_total];

Sprite const *sprite_thinking = nullptr;

Sprite const *sprite_health_box = nullptr;
Sprite const *sprite_exit = nullptr;
Sprite const *sprite_tile[SCENE_TOTAL][TILE_TOTAL];
Sprite const *sprite_instruction_panel = nullptr;
Sprite const *sprite_helper = nullptr;
Sprite const *sprite_recipe = nullptr;
Sprite const *sprite_pot_normal = nullptr;
Sprite const *sprite_pot_cooking = nullptr;
Sprite const *sprite_fire = nullptr;
Sprite const *sprite_black = nullptr;
Sprite const *sprite_add = nullptr;
Sprite const *sprite_unlock = nullptr;
Sprite const *sprite_jump = nullptr;

Load< SpriteAtlas > sprites(LoadTagDefault, []() -> SpriteAtlas const * {
	SpriteAtlas const *ret = new SpriteAtlas(data_path("cookmap"));

	for (int i = 0; i < SCENE_TOTAL; ++i) {
        sprite_background[i] = &ret->lookup("background_" + to_string(i+1));
	}
    sprite_chef_left_stand = &ret->lookup("chef_lstand");
    sprite_chef_left_walk1 = &ret->lookup("chef_lwalk1");
    sprite_chef_left_walk2 = &ret->lookup("chef_lwalk2");
    sprite_chef_left_jump = &ret->lookup("chef_ljump");
    sprite_chef_right_stand = &ret->lookup("chef_rstand");
    sprite_chef_right_walk1 = &ret->lookup("chef_rwalk1");
    sprite_chef_right_walk2 = &ret->lookup("chef_rwalk2");
    sprite_chef_right_jump = &ret->lookup("chef_rjump");

    sprite_item_1 = &ret->lookup("item_1");
    sprite_item_2 = &ret->lookup("item_2");
    sprite_item_3 = &ret->lookup("item_3");
    sprite_item_4 = &ret->lookup("item_4");
    sprite_item_5 = &ret->lookup("item_5");
    sprite_item_6 = &ret->lookup("item_6");
    sprite_item_7 = &ret->lookup("item_7");
    sprite_item_8 = &ret->lookup("item_8");
    sprite_item_9 = &ret->lookup("item_9");
    sprite_item_10 = &ret->lookup("item_10");
    sprite_item_11 = &ret->lookup("item_11");
    sprite_item_12 = &ret->lookup("item_12");
    sprite_item_13 = &ret->lookup("item_13");
    sprite_item_14 = &ret->lookup("item_14");
    sprite_item_15 = &ret->lookup("item_15");
    sprite_item_16 = &ret->lookup("item_16");
    sprite_item_17 = &ret->lookup("item_17");
    sprite_item_18 = &ret->lookup("item_18");
    sprite_item_19 = &ret->lookup("item_19");
    sprite_item_20 = &ret->lookup("item_20");
    sprite_item_21 = &ret->lookup("item_21");
    sprite_item_22 = &ret->lookup("item_22");

    sprite_dish_0 = &ret->lookup("waste");
    sprite_dish_1 = &ret->lookup("dish_1");
    sprite_dish_2 = &ret->lookup("dish_2"); 
    sprite_dish_3 = &ret->lookup("dish_3"); 
    sprite_dish_4 = &ret->lookup("dish_4"); 
    sprite_dish_5 = &ret->lookup("dish_5"); 
    sprite_dish_6 = &ret->lookup("dish_6"); 
    sprite_dish_7 = &ret->lookup("dish_7"); 
    sprite_dish_8 = &ret->lookup("dish_8"); 
    sprite_dish_9 = &ret->lookup("dish_9");

    for (int i = 0; i < npc_total; ++i) {
        sprite_npc[i] = &ret->lookup(string("guard_") + to_string(i+1));
        sprite_npc_idle[i] = &ret->lookup(string("guard_") + to_string(i+1) + "_idle");
    }
    sprite_thinking = &ret->lookup("thinking");

    sprite_health_box = &ret->lookup("health_box");
    sprite_exit = &ret->lookup("exit");
    for (int i = 0; i < SCENE_TOTAL; ++i) {
        for (int j = 0; j < TILE_TOTAL; ++j) {
            sprite_tile[i][j] =  &ret->lookup("tile_" + to_string(i*TILE_TOTAL+j+1));
        }
    }
    sprite_instruction_panel = &ret->lookup("panel_1");
    sprite_helper = &ret->lookup("help");
    sprite_recipe = &ret->lookup("help");
    sprite_pot_normal = &ret->lookup("pot_normal");
    sprite_pot_cooking = &ret->lookup("pot_cooking");
    sprite_fire = &ret->lookup("bonfire");
    sprite_item_question = &ret->lookup("question");
    sprite_black = &ret->lookup("black");
    sprite_add = &ret->lookup("add");
    sprite_unlock = &ret->lookup("unlock");
    sprite_jump = &ret->lookup("jump");
	return ret;
});

//////////////////////////////////////////////// music //////////////////////////////////////////////////

Load< Sound::Sample > icon_click(LoadTagDefault, []() -> Sound::Sample *{
    std::vector< float > data(size_t(48000 * 0.2f), 0.0f);
    for (uint32_t i = 0; i < data.size(); ++i) {
        float t = i / float(48000);
        //phase-modulated sine wave (creates some metal-like sound):
        data[i] = std::sin(3.1415926f * 2.0f * 440.0f * t + std::sin(3.1415926f * 2.0f * 450.0f * t));
        //quadratic falloff:
        data[i] *= 0.3f * std::pow(std::max(0.0f, (1.0f - t / 0.2f)), 2.0f);
    }
    return new Sound::Sample(data);
});

Load< Sound::Sample > icon_clonk(LoadTagDefault, []() -> Sound::Sample *{
    std::vector< float > data(size_t(48000 * 0.2f), 0.0f);
    for (uint32_t i = 0; i < data.size(); ++i) {
        float t = i / float(48000);
        //phase-modulated sine wave (creates some metal-like sound):
        data[i] = std::sin(3.1415926f * 2.0f * 220.0f * t + std::sin(3.1415926f * 2.0f * 200.0f * t));
        //quadratic falloff:
        data[i] *= 0.3f * std::pow(std::max(0.0f, (1.0f - t / 0.2f)), 2.0f);
    }
    return new Sound::Sample(data);
});

Load< Sound::Sample > music_collision(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("collision.opus"));
});

Load< Sound::Sample > music_eat(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("eat.opus"));
});

Load< Sound::Sample > music_jump(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("jump.opus"));
});

Load< Sound::Sample > music_oh_no(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("oh_no.opus"));
});

Load< Sound::Sample > music_pickup(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("pickup.opus"));
});

Load< Sound::Sample > music_pleasure(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("pleasure.opus"));
});

Load< Sound::Sample > music_pot_cook(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("pot_cook.opus"));
});

Load< Sound::Sample > music_pot_finish(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("pot_finish.opus"));
});

Load< Sound::Sample > music_pot_put(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("pot_put.opus"));
});

Load< Sound::Sample > music_scene1_bgm(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("scene1_bgm.opus"));
});

Load< Sound::Sample > music_scene2_bgm(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("scene2_bgm.opus"));
});

Load< Sound::Sample > music_scene3_bgm(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("scene3_bgm.opus"));
});

Load< Sound::Sample > music_trash(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("trash.opus"));
});

Load< Sound::Sample > music_win(LoadTagDefault, []() -> Sound::Sample * {
    return new Sound::Sample(data_path("win.opus"));
});

///////////////////////////////////////////////////// map//////////////////////////////////////


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
    int npc_num, npc_type_, npc_dish_;
    float npc_x, npc_y;

    for (unsigned i = 0; i < mode->npcs.size(); ++i) {
        delete mode->npcs[i];
    }
    mode->npcs.clear();

    f >> npc_num;
    mode->npcs.resize(npc_num);
    for (int i = 0; i < npc_num; ++i) {
        f >> npc_type_ >> npc_x >> npc_y >> npc_dish_;
        mode->npcs[i] = new Npc((npc_type)npc_type_, TILE_SIZE*npc_x, TILE_SIZE*npc_y, (dish_type)npc_dish_);
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
                case 'g': case 'w': case 'l': case 'r': case 'z': case 'x': case 'c': case 'v':
                    part = new Ground(s[x]);
                    break;
                case '~':
                    part = new Goal();
                    break;
                case '1': case '2': case '3': case '4': 
                case '5': case '6': case '7': case '8': case '9': 
                    part = new Ingredient((ingredient_type)(s[x] - '1'));
                    break;
                case '0':
                    part = new Ingredient((ingredient_type)(9));
                    break;
                case '!':
                    part = new Ingredient((ingredient_type)(10));
                    break;
                case '@':
                    part = new Ingredient((ingredient_type)(11));
                    break;
                case '#':
                    part = new Ingredient((ingredient_type)(12));
                    break;
                case '$':
                    part = new Ingredient((ingredient_type)(13));
                    break;
                case '%':
                    part = new Ingredient((ingredient_type)(14));
                    break;
                case '^':
                    part = new Ingredient((ingredient_type)(15));
                    break;
                case '&':
                    part = new Ingredient((ingredient_type)(16));
                    break;
                case '*':
                    part = new Ingredient((ingredient_type)(17));
                    break;
                case '(':
                    part = new Ingredient((ingredient_type)(18));
                    break;
                case ')':
                    part = new Ingredient((ingredient_type)(19));
                    break;
                case '[':
                    part = new Ingredient((ingredient_type)(20));
                    break;
                 case ']':
                    part = new Ingredient((ingredient_type)(21));
                    break;
                default:
                    cerr << "Error: map type " << s[x] << " unrecognized" << endl;
                    return false;
            }
            part->set_pos(TILE_SIZE*x, TILE_SIZE*(height-1-y));
        }
    }

    f.close();
    return true;
}

StoryMode::StoryMode() {
    ingredient_map.insert ( {{Item1,*sprite_item_1},{Item2,*sprite_item_2},
        {Item3,*sprite_item_3},{Item4,*sprite_item_4},{Item5,*sprite_item_5},{Item6,*sprite_item_6},
        {Item7,*sprite_item_7},{Item8,*sprite_item_8},{Item9,*sprite_item_9},{Item10,*sprite_item_10},
        {Item11,*sprite_item_11},{Item12,*sprite_item_12},{Item13,*sprite_item_13},{Item14,*sprite_item_14},
        {Item15,*sprite_item_15},{Item16,*sprite_item_16},{Item17,*sprite_item_17},{Item18,*sprite_item_18},
        {Item19,*sprite_item_19}, {Item20,*sprite_item_20}, {Item21,*sprite_item_21}, {Item22,*sprite_item_22}
    }); 
    dish_map.insert ( {{Dish0, *sprite_dish_0},{Dish1, *sprite_dish_1},{Dish2, *sprite_dish_2},{Dish3, *sprite_dish_3},
        {Dish4, *sprite_dish_4},{Dish5, *sprite_dish_5}, {Dish6, *sprite_dish_6}, {Dish7, *sprite_dish_7}
        ,{Dish8, *sprite_dish_8}, {Dish9, *sprite_dish_9}} ); 

    load_map_file(data_path("map_" + to_string(scene_num+1) + ".txt"), this);
    last_time = timepoint;
    save_state(this);
}

StoryMode::~StoryMode() {
}

void StoryMode::save_state(StoryMode* mode) {
    mode->player_b.health = mode->player.health;
    mode->backpack_b = mode->backpack;
    mode->dishes_b = mode->dishes;
    mode->pots_b = mode->pots;
}

void StoryMode::load_state(StoryMode* mode) {
    mode->player.health = mode->player_b.health;
    mode->backpack = mode->backpack_b;
    mode->dishes = mode->dishes_b;
    mode->pots = mode->pots_b;
}

void StoryMode::restart(StoryMode* mode) {
    load_map_file(data_path("map_" + to_string(scene_num+1) + ".txt"), mode);
    last_time = timepoint;
    load_state(mode);
    mode->power_map = mode->power_map_b;
}

bool StoryMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
    if (winning || lose) {
        return false;
    }
    bool res=false;
	if (evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP) {
		if (evt.key.keysym.scancode == SDL_SCANCODE_A) {
			controls.left = (evt.type == SDL_KEYDOWN);
			res = true;
		}
		if (evt.key.keysym.scancode == SDL_SCANCODE_D) {
			controls.right = (evt.type == SDL_KEYDOWN);
			res = true;
		}
		if (evt.key.keysym.scancode == SDL_SCANCODE_W ||
		           evt.key.keysym.scancode == SDL_SCANCODE_SPACE) {
            if (player.state != Left_jump && player.state != Right_jump) {
                if(controls.up==false && evt.type == SDL_KEYDOWN)
                    Sound::play(*music_jump,0.2);
                controls.up = (evt.type == SDL_KEYDOWN);
			    res=true;
            }
		}
	}

    if (evt.type== SDL_MOUSEBUTTONDOWN){
        if (evt.button.x>= dish_start_x && evt.button.x<=dish_start_x+item_inteval*(dish_num-1)+item_size &&
           (evt.button.x-dish_start_x)%item_inteval<item_size && evt.button.y>=dish_start_y && evt.button.y<=dish_start_y+item_size &&
           int((evt.button.x-dish_start_x)/item_inteval) <int(dishes.size() ) ) {
            dragged_dish=dishes[(evt.button.x-dish_start_x)/item_inteval];
            dishes.erase(dishes.begin() + (evt.button.x-dish_start_x)/item_inteval );
            dish_drag = true;
            dish_drag_pos = glm::vec2(evt.button.x,draw_width-evt.button.y)+view_min;
        } else if (evt.button.x>= item_start_x && evt.button.x<=item_start_x+item_inteval*(item_num-1)+item_size &&
                   (evt.button.x-item_start_x)%item_inteval<item_size && evt.button.y>=item_start_y && evt.button.y<=item_start_y+item_size &&
                   int((evt.button.x-item_start_x)/item_inteval) <int(backpack.size() ) ) {
            // drag from backpack
            dragging_ingre_type = backpack[(evt.button.x-item_start_x)/item_inteval];
            backpack.erase(backpack.begin() + (evt.button.x-item_start_x)/item_inteval );
            ingre_drag = true;
            drag_from_backpack = true;
            ingre_drag_pos = glm::vec2(evt.button.x,draw_width-evt.button.y)+view_min;
        } else if (evt.button.x>= pot_x && evt.button.x<=pot_x+item_size &&
                   (evt.button.y-77)%60<item_size && evt.button.y>=77 && evt.button.y<=245 &&
                   int((evt.button.y-77)/60) <int(pots.size() ) ) {
            // drag from pot
            dragging_ingre_type = pots[(evt.button.y-77)/60];
            pots.erase(pots.begin() + (evt.button.y-77)/60 );
            ingre_drag = true;
            drag_from_backpack = false;
            ingre_drag_pos = glm::vec2(evt.button.x,draw_width-evt.button.y)+view_min;
        } else if (evt.button.x>= help_x && evt.button.x<=help_x+item_size &&
                   evt.button.y>=help_y && evt.button.y<=help_y+item_size) {

                Sound::play(*icon_click);
            show_instruction =! show_instruction;
            show_pot = false;
            show_recipe = false;
        } else if (evt.button.x>= recipe_x && evt.button.x<=recipe_x+item_size &&
                   evt.button.y>=recipe_y && evt.button.y<=recipe_y+item_size) {
                Sound::play(*icon_click);
            show_recipe = !show_recipe;
            show_instruction = false;
        } else if (evt.button.x>= pot_x && evt.button.x<=pot_x+item_size  &&
                   evt.button.y>=pot_y && evt.button.y<=pot_y+item_size) {
                Sound::play(*icon_click);      
            show_pot = !show_pot;
            show_instruction = false;
        } else if (evt.button.x>= pot_x && evt.button.x<=pot_x+item_size  &&
                   evt.button.y>=263 && evt.button.y<=311 &&
                   show_pot && pot_time_left == 0.f && pots.size()) {
            Sound::play(*icon_click);
            pot_time_left = 500.f;
        }
        res=true;
    }
    if(evt.type== SDL_MOUSEMOTION && dish_drag){
        dish_drag_pos=glm::vec2(evt.motion.x,draw_width-evt.motion.y)+view_min;
        res=true;
    }
    if (evt.type== SDL_MOUSEBUTTONUP && dish_drag){
        dish_drag=false;

        if (collision(dish_drag_pos, glm::vec2(48,48), player.position, player.radius)) {
            // eat dish
            Sound::play(*music_eat);
            if (power_map.find(dragged_dish) != power_map.end()) {
                bool unlock_one = false;
                switch (power_map[dragged_dish])
                {
                case 1:
                    for (unsigned i = 0; i < recipes.size(); ++i) {
                        for (unsigned j = 0; j < recipes[i].ingredients.size(); ++j) {
                            if (!recipes[i].show[j]) {
                                recipes[i].show[j] = true;
                                unlock_one = true;
                                break;
                            }
                        }
                        if (unlock_one)
                            break;
                    }
                    break;
                case 2:
                    player.big_jump = true;
                    power_time = timepoint;
                default:
                    break;
                }
                power_map.erase(dragged_dish);
            } else {
                player.health += health_map[dragged_dish];
                if (player.health > 10) {
                    player.health = 10;
                }
            }
        } else if (dish_drag_pos.x >= garbage_x+view_min.x && dish_drag_pos.x <= garbage_x+item_size+view_min.x  &&
            (draw_width-dish_drag_pos.y) >= garbage_y+view_min.y && (draw_width-dish_drag_pos.y) <= garbage_y+item_size+view_min.y) {
            Sound::play(*music_trash);
            // discard dish
        } else {
            // bribe npc 
            auto eaten = false;
            for (unsigned i = 0; i < npcs.size(); i++) {
                if(collision(dish_drag_pos, glm::vec2(item_size,item_size), npcs[i]->position, npcs[i]->radius)){
                    for (unsigned j = 0; j < npcs[i]->favorates.size(); j++) {
                        if (npcs[i]->favorates[j] == dragged_dish) {
                            Sound::play(*music_pleasure);
                            npcs[i]->eat=true;
                            eaten = true;
                            break;
                        }
                    }
                }
                if (eaten)
                    break;
            }

            if (!eaten)
                dishes.push_back(dragged_dish);
        }
        res=true;
    }
    if(evt.type== SDL_MOUSEMOTION && ingre_drag){
        ingre_drag_pos=glm::vec2(evt.motion.x,draw_width-evt.motion.y)+view_min;
        res=true;
    }
    if (evt.type== SDL_MOUSEBUTTONUP && ingre_drag){
        ingre_drag=false;
        if (ingre_drag_pos.x >= pot_x+view_min.x && ingre_drag_pos.x <= pot_x+item_size+view_min.x  &&
            ingre_drag_pos.y >= 513.f+view_min.y && ingre_drag_pos.y <= 690.f+view_min.y && show_pot && pots.size() < 3) {
            // drag to pot
            Sound::play(*music_pot_put);
            pots.push_back(dragging_ingre_type);
        } else if (evt.button.x>= item_start_x && evt.button.x<=item_start_x+item_inteval*(item_num-1)+item_size &&
                   (evt.button.x-item_start_x)%item_inteval<item_size && evt.button.y>=item_start_y && evt.button.y<=item_start_y+item_size &&
                   int((evt.button.x-item_start_x)/item_inteval) >= int(backpack.size() ) ) {
            // drag to backpack
            backpack.push_back(dragging_ingre_type);
        } else if (ingre_drag_pos.x >= garbage_x+view_min.x && ingre_drag_pos.x <= garbage_x+item_size+view_min.x  &&
            (draw_width-ingre_drag_pos.y) >= garbage_y+view_min.y && (draw_width-ingre_drag_pos.y) <= garbage_y+item_size+view_min.y) {
            Sound::play(*music_trash);
            // discard ingredient
        } else{
            if (drag_from_backpack) {
                backpack.push_back(dragging_ingre_type);
            } else {
                pots.push_back(dragging_ingre_type);
            }
        }
        res=true;
    }

        return res;
}

void StoryMode::update(float elapsed) {
	if (Mode::current.get() == this) {
		//there is no menu displayed! Make one:
		enter_scene(elapsed);
	}

    if (!background_music || background_music->stopped) {
        switch(scene_num){
            case 0:
                background_music = Sound::play(*music_scene1_bgm, 0.5);
                break;
            case 1:
                background_music = Sound::play(*music_scene2_bgm, 0.5);
                break;
            case 2:
                background_music = Sound::play(*music_scene3_bgm, 0.5);
                break;              
        }
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
            velocity.y = -16.0f;
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
    timepoint += elapsed;
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
                    if (npcs[i]->eat) {
                        velocity_i.x = 0;
                        position_i = position_i + velocity_i * elapsed;
                        if (position_i.y <= init_position_i.y) {
                            position_i.y = init_position_i.y;
                            velocity_i.y = -velocity_i.y;
                        } else if (position_i.y > init_position_i.y 
                            && position_i.y - init_position_i.y > 240.0f) {
                            velocity_i.y = -velocity_i.y;
                        }
                    } else {
                        position_i = position_i + velocity_i * elapsed;
                        if (position_i.x <= init_position_i.x) {
                            position_i.x = init_position_i.x;
                            velocity_i.x = -velocity_i.x;
                        } else if (position_i.x > init_position_i.x 
                            && position_i.x - init_position_i.x > 140.0f) {
                            velocity_i.x = -velocity_i.x;
                        }
                        position_i.y = init_position_i.y;
                    }
                    break;
                case npc1:
                    if (npcs[i]->eat) {
                        velocity_i.x = 0;
                        position_i = position_i + velocity_i * elapsed;
                        if (position_i.y <= init_position_i.y) {
                            position_i.y = init_position_i.y;
                            velocity_i.y = -velocity_i.y;
                        } else if (position_i.y > init_position_i.y 
                            && position_i.y - init_position_i.y > 100.0f) {
                            velocity_i.y = -velocity_i.y;
                        }
                    } else {
                        position_i = position_i + velocity_i * elapsed;
                        if (position_i.x <= init_position_i.x) {
                            position_i.x = init_position_i.x;
                            velocity_i.x = -velocity_i.x;
                        } else if (position_i.x > init_position_i.x 
                            && position_i.x - init_position_i.x > 30.0f) {
                            velocity_i.x = -velocity_i.x;
                        }
                        position_i.y = init_position_i.y;
                    }
                    break;
                case npc2:
                    if (npcs[i]->eat) {
                        velocity_i.x = 0;
                        position_i = position_i + velocity_i * elapsed;
                        if (position_i.y <= init_position_i.y) {
                            position_i.y = init_position_i.y;
                            velocity_i.y = -velocity_i.y;
                        } else if (position_i.y > init_position_i.y 
                            && position_i.y - init_position_i.y > 240.0f) {
                            velocity_i.y = -velocity_i.y;
                        }
                    } else {
                        position_i = position_i + velocity_i * elapsed;
                        velocity_i.x=velocity_i.x/abs(velocity_i.x)*(50+abs(position_i.x-init_position_i.x));
                        if (position_i.x >= init_position_i.x) {
                            position_i.x = init_position_i.x;
                            velocity_i.x = -velocity_i.x;
                        } else if (position_i.x < init_position_i.x 
                            && init_position_i.x - position_i.x > 200.0f) {
                            velocity_i.x = -velocity_i.x;
                        }
                        position_i.y = init_position_i.y;
                    }
                    break;
                case npc3:
                    if (npcs[i]->eat) {
                        velocity_i.x = 0;
                        position_i = position_i + velocity_i * elapsed;
                        if (position_i.y <= init_position_i.y) {
                            position_i.y = init_position_i.y;
                            velocity_i.y = -velocity_i.y;
                        } else if (position_i.y > init_position_i.y 
                            && position_i.y - init_position_i.y > 240.0f) {
                            velocity_i.y = -velocity_i.y;
                        }
                    } else {
                        position_i = position_i + velocity_i * elapsed;
                        velocity_i.x=velocity_i.x/abs(velocity_i.x)*(50+abs(position_i.x-init_position_i.x));
                        if (position_i.x <= init_position_i.x) {
                            position_i.x = init_position_i.x;
                            velocity_i.x = -velocity_i.x;
                        } else if (position_i.x > init_position_i.x 
                            && position_i.x - init_position_i.x  > 200.0f) {
                            velocity_i.x = -velocity_i.x;
                        }
                        position_i.y = init_position_i.y;
                    }
                    break;
                case npc4:
                    if (npcs[i]->eat) {
                        velocity_i.x = 0;
                        position_i = position_i + velocity_i * elapsed;
                        if (position_i.y <= init_position_i.y) {
                            position_i.y = init_position_i.y;
                            velocity_i.y = -velocity_i.y;
                        } else if (position_i.y > init_position_i.y 
                            && position_i.y - init_position_i.y > 100.0f) {
                            velocity_i.y = -velocity_i.y;
                        }
                    } else {
                        if (velocity_i.x > 0) {
                            velocity_i.x=velocity_i.x/abs(velocity_i.x)*(50+abs(position_i.x-init_position_i.x-800.0f));
                        } else {
                            velocity_i.x = -90.0f;
                        }
                        position_i = position_i + velocity_i * elapsed;
                        if (position_i.x <= init_position_i.x - 900.0f) {
                            position_i.x = init_position_i.x - 900.0f;
                            velocity_i.x = -velocity_i.x;
                        } else if (position_i.x - init_position_i.x > 170.0f) {
                            position_i.x = init_position_i.x + 170.0f;
                            velocity_i.x = -velocity_i.x;
                        }
                        position_i.y = init_position_i.y;
                    }
                    break;
                default:
                    break;
            }
        }

		//player motion:
		//build a shove from controls:
        glm::vec2 &position = player.position;
        glm::vec2 &velocity = player.velocity;
        glm::vec2 &radius = player.radius;

		glm::vec2 shove = glm::vec2(0.0f);
		if (controls.left) shove.x -= 28.0f;
		if (controls.right) shove.x += 28.0f;
        jump_interval = max(0.f, jump_interval - elapsed);
        if (player.state != Left_jump && player.state != Right_jump && jumping) {
            jump_interval = 0.1f;
            jumping = false;
        }
		if (controls.up && abs(velocity.y) < 1e-4 && jump_interval == 0.f) {
            if (player.big_jump) {
                // Super jump
                shove.y += 60.5f;
                curt_time = timepoint;
                if (curt_time - power_time >= 5.f) {
                    player.big_jump = false;
                }
            } else {
                shove.y += 40.5f;
            }
            jumping = true;
            controls.up = false;
		}
		shove *= 10.0f;

        auto old_velocity = glm::vec2(velocity.x, velocity.y);
		auto tmp = glm::mix(shove, velocity, std::pow(0.5f, elapsed / 0.25f));
        if (shove.y > 0.0f || player.state == Left_jump || player.state == Right_jump) {
            velocity = glm::vec2(tmp.x, velocity.y + shove.y - 740.0f * elapsed);
        } else {
            velocity = glm::vec2(tmp.x, velocity.y + shove.y - 260.0f * elapsed);
        }
		
        if (old_velocity.y > 0.0f && velocity.y < 0.0f && velocity.y > -16.0f) {
            velocity = glm::vec2(velocity.x, -16.0f);
        }
        // bound the negative velocity to avoid collision error
        if (velocity.y < -500.0f) {
            velocity = glm::vec2(velocity.x, -500.0f);
        }

        curt_time = timepoint;
        auto time_since_last = (curt_time - last_time) * 1000;
        // change player state to draw animations
        if (velocity.y > 0.0f || velocity.y <= -16.0f) {
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
                if (player.state == Right_walk1) {
                    if (time_since_last > 300) {
                        player.state = Right_walk2;
                        last_time = timepoint;
                    } 
                } else {
                    if (time_since_last > 300) {
                        player.state = Right_walk1;
                        last_time = timepoint;
                    }
                }
            } else if (velocity.x < 0.0f && velocity.x > -20.0f) {
                player.state = Left_stand;
            } else {
                if (player.state == Left_walk1) {
                    if (time_since_last > 300) {
                        player.state = Left_walk2;
                        last_time = timepoint;
                    }
                } else {
                    if (time_since_last > 300) {
                        player.state = Left_walk1;
                        last_time = timepoint;
                    }
                }
            }
        }

        position = position + velocity * elapsed * (float)0.5;

        //---- collision handling ----
        // Npc detection
        for (unsigned i = 0; i < npcs.size(); i++) {
            glm::vec2 box = npcs[i]->position;
            glm::vec2 box_radius = npcs[i]->radius;
            if(npcs[i]->eat)
                continue;
            if (collision(position, radius, box, box_radius)) {
                resolve_collision(position, radius, box, box_radius, velocity);
//                player.health -= npcs[i]->attack;
//                player.health = max(0,player.health);

                Sound::play(*music_collision,0.5);
                if (position.x <= npcs[i]->position.x)
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
                        if (!ingre->obtained && ((int)backpack.size() < item_num-1 ||
                            ((int)backpack.size() == item_num-1 && !ingre_drag))) {
                            ingre->obtained = true;
                            Sound::play(*music_pickup,0.5);
                            backpack.push_back(ingre->type);
                        }
						break;

                    case part_goal_type:
                        if (!lose) {
                            if (scene_num + 1 < SCENE_TOTAL && scene_transition == 3.f) {
                                scene_target = scene_num + 1;
                                scene_transition = 0.f;
                            } else if (scene_num + 1 == SCENE_TOTAL){
                                winning = true;
                            }
                        }
                        break;

					default:
						break;
					}
				}
			}
		}

        position = position + velocity * elapsed * (float)0.5;
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
                        if (!ingre->obtained && ((int)backpack.size() < item_num-1 ||
                            ((int)backpack.size() == item_num-1 && !ingre_drag))) {
                            ingre->obtained = true;
                            Sound::play(*music_pickup,0.5);
                            backpack.push_back(ingre->type);
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
        // restart if fall down
        if (position.y < 0) {
            restart(this);
            return;
        }
	}
	{
        // slide viewport window
        float left_border = max(player.position.x - 512.0f, 0.0f);
        left_border = min(left_border, parts[0].size() * TILE_SIZE - 1024);

        view_min = glm::vec2(int(left_border), 0);
        view_max = glm::vec2(int(left_border) + 1024, view_max.y);
    }



    if (pot_time_left > 0.f) {
        if (pot_time_left == 500.0f) {
            // check making dish
            pot_time_left=5;
            cooking_dish=Dish0;
            unordered_map<ingredient_type, int> num;
            int health_cost = 0;
            for (auto i : pots) {
                num[i]++;
                health_cost += ingre_cost[i];
            }
            if (player.health >= health_cost && dishes.size() < 2) {
                player.health -= health_cost;
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
                        cooking_recipe=&recipe;
                        cooking_dish = recipe.dish;
                        pot_time_left = recipe.cost;
                        break;
                    }
                }
                Sound::play(*music_pot_cook);
                pots.clear();
            } else {
                pot_time_left=0;
            }
        }
        if (pot_time_left!=0) {
            pot_time_left -= elapsed;
            if (pot_time_left <= 0.f) {
                if(cooking_dish!=Dish0)
                    Sound::play(*music_pot_finish);
                else
                    Sound::play(*music_oh_no);
                dishes.push_back(cooking_dish);
                pot_time_left = 0.f;
                if(cooking_dish!=Dish0){
                    for(auto show:cooking_recipe->show)
                        show=true;
                }
            }
        }
    }

    scene_transition = min(3.f, scene_transition + elapsed);
}



void StoryMode::draw(glm::uvec2 const &drawable_size) {
                //health_pos = glm::vec2(68.0f, 724.0f);
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
            draw.draw(*sprite_background[scene_num], bl);

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
                                draw.draw(*sprite_tile[scene_num][0], parts[i][j]->position);
                                break;
                            case 'w':
                                draw.draw(*sprite_tile[scene_num][1], parts[i][j]->position);
                                break;
                            case 'l':
                                draw.draw(*sprite_tile[scene_num][2], parts[i][j]->position);
                                break;
                            case 'r':
                                draw.draw(*sprite_tile[scene_num][3], parts[i][j]->position);
                                break;
                            case 'z':
                                draw.draw(*sprite_tile[scene_num][4], parts[i][j]->position);
                                break;
                            case 'x':
                                draw.draw(*sprite_tile[scene_num][5], parts[i][j]->position);
                                break;
                            case 'c':
                                draw.draw(*sprite_tile[scene_num][6], parts[i][j]->position);
                                break;
                            case 'v':
                                draw.draw(*sprite_tile[scene_num][7], parts[i][j]->position);
                                break;
                            }
                            break;

                        case part_empty_type:
                            break;

                        case part_ingredient_type:
                            ingre = reinterpret_cast<Ingredient*>(parts[i][j]);
                            if (!ingre->obtained) {
                                draw.draw(ingredient_map[ingre->type], parts[i][j]->position);
                            } else {
                                draw.draw(ingredient_map[ingre->type], glm::vec2(parts[i][j]->position.x, parts[i][j]->position.y - item_size + ingre->grow_count));
                                ingre->grow_count += 0.02;
                                if (ingre->grow_count >= item_size) {
                                    ingre->grow_count = item_size;
                                    ingre->obtained = false;
                                    ingre->grow_count = 0;
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
                if (npcs[i]->eat)
                    draw.draw(*sprite_npc_idle[npcs[i]->type], npcs[i]->position);
                else{
                    draw.draw(*sprite_npc[npcs[i]->type], npcs[i]->position);
                    draw.draw(*sprite_thinking,
                              glm::vec2(npcs[i]->position.x-50, npcs[i]->position.y+120));
                    draw.draw(dish_map[npcs[i]->favorates[0]],
                              glm::vec2(npcs[i]->position.x-35, npcs[i]->position.y+145));
                }
            }

            for (unsigned i = 0; i < backpack.size(); i++) {
                draw.draw(ingredient_map[backpack[i]], backpack_pos[i]+view_min);
                for (int j = ingre_cost[backpack[i]]; j > 0; --j) {
                    draw.draw(*sprite_health_box, backpack_pos[i]+view_min+glm::vec2(43-j*8, 4), 0.4);
                }
            }

            for (unsigned i = 0; i < dishes.size(); i++) 
                draw.draw(dish_map[dishes[i]],  dishes_pos[i]+view_min);

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
            


            for (int h = 0; h < player.health; h++) {
                glm::vec2 current_health_pos=health_pos+view_min;
                current_health_pos.x+=h*20.0f;
                draw.draw(*sprite_health_box, current_health_pos);
            }

            if (winning) 
                draw.draw_text("YOU  W I N!", glm::vec2(160.0f, 330.0f)+view_min, 0.4);
            if (winning && !winned){
                Sound::play(*music_win);
                winned=true;
            }

            if (show_instruction) {
                draw_instruction(draw);
            }

            if (show_recipe) {
                draw_recipe(draw);
            }

            if(pot_time_left>0)
                draw.draw(*sprite_pot_cooking, glm::vec2(pot_x,draw_width-pot_y-item_size)+view_min);
            else
                draw.draw(*sprite_pot_normal, glm::vec2(pot_x,draw_width-pot_y-item_size)+view_min);
            if (show_pot) {
                draw_pot(draw);
            }

            if(dish_drag){
                draw.draw(dish_map[dragged_dish], dish_drag_pos);
            }

            if(ingre_drag){
                draw.draw(ingredient_map[dragging_ingre_type] , ingre_drag_pos);
            }

            if (scene_transition < 0.5f) {
                draw.draw(*sprite_black, bl, 1.0f,
                          glm::u8vec4(0xff, 0xff, 0xff, (unsigned char)(255.f*scene_transition/0.5f)));
            } else if (scene_transition < 2.5f) {
                draw.draw(*sprite_black, bl);
            } else if (scene_transition < 3.f){
                draw.draw(*sprite_black, bl, 1.0f,
                          glm::u8vec4(0xff, 0xff, 0xff, (unsigned char)(255.f*(3.f-scene_transition)/0.5f)));
            }
            if (scene_transition >1.5f && scene_num != scene_target) {
                player.health = 10;
                save_state(this);
                background_music->stop();
                scene_num = scene_target;
                Sound::play(*music_win);
                restart(this);
            }
            if (scene_transition < 3.f) {
                draw.draw_text("ENTERING   LEVEL   " + to_string(scene_target+1), glm::vec2(130.0f, 350.0f)+view_min, 0.2);
            }

		} else {
            // cooking
		}

	}
	GL_ERRORS(); //did the DrawSprites do something wrong?
}


void StoryMode::draw_instruction(DrawSprites& draw) {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 5; ++j) {
            draw.draw(*sprite_instruction_panel,
                    glm::vec2(draw_length-7-item_size*(i+1), 615-item_size*j)+view_min);
        }
    }
    draw.draw_text(
            "a,d   to   move,   w, space to jump\n"
            "click   to   open   the recipe\n"
            "open   pot   and   drag   items   i n\n"
            "click   fire   to   cook\n"
            "drag   dish   to   enemy   to   bribe\n"
            "drag   dish   to   self   to   heal\n"
            "click   bulb   to   close   help",
            glm::vec2(draw_length-item_size*9, 630)+view_min, 0.068);
}

void StoryMode::draw_pot(DrawSprites& draw) {
    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 5; ++j) {
            draw.draw(*sprite_instruction_panel,
                      glm::vec2(pot_x+item_size*i, 645-item_size*j)+view_min);
        }
    }
    draw.draw(*sprite_tile[scene_num][0], glm::vec2(pot_x, 510.f)+view_min, glm::vec2(1.f, 0.05f), glm::u8vec4(0x00, 0x00, 0x00, 0xff));
    if (pot_time_left > 0.f) {
        string tmp = std::to_string(int(pot_time_left+0.999f));
        draw.draw_text(tmp, glm::vec2(pot_x+15, 461.f)+view_min, 0.07);
    } else {
        draw.draw(*sprite_fire, glm::vec2(pot_x, 457.f)+view_min);
    }

    for (unsigned i = 0; i < pots.size(); ++i) {
        auto pos = glm::vec2(pot_x, 643-60.f*i)+view_min;
        draw.draw(ingredient_map[pots[i]], pos);
        for (int j = ingre_cost[pots[i]]; j > 0; --j) {
            draw.draw(*sprite_health_box, glm::vec2(pot_x, 643-60.f*i)+view_min+glm::vec2(43-j*8, 4), 0.4);
        }
    }
}

void StoryMode::draw_recipe(DrawSprites& draw) {
    for (int i = 0; i < 8; ++i) {
        for (unsigned j = 0; j < 1.3* recipes.size(); ++j) {
            draw.draw(*sprite_instruction_panel,
                      glm::vec2(665-216+item_size*i, 645-item_size*j)+view_min);
        }
    }

    for (unsigned i = 0; i < recipes.size(); ++i) {
        for (unsigned j = 0; j < recipes[i].ingredients.size(); ++j) {
            auto pos = glm::vec2(677-216+60.f*j, 631-60.f*i)+view_min;
            if (recipes[i].show[j]) {
                draw.draw(ingredient_map[recipes[i].ingredients[j]], pos);
                for (int k = ingre_cost[recipes[i].ingredients[j]]; k > 0; --k) {
                    draw.draw(*sprite_health_box, pos+glm::vec2(43-k*8, 4), 0.4);
                }
            } else {
                draw.draw(*sprite_item_question, pos);
            }
        }
        auto pos = glm::vec2(865.f-216, 631-60.f*i)+view_min;
        draw.draw(*sprite_tile[scene_num][0], pos, glm::vec2(0.05f, 1.0f), glm::u8vec4(0x00, 0x00, 0x00, 0xff));
        pos = glm::vec2(885.f-216, 631-60.f*i)+view_min;
        draw.draw(dish_map[recipes[i].dish], pos);
        draw.draw(*sprite_add, pos+glm::vec2(60, 0), 0.5);      
        if(power_map.count(recipes[i].dish)!=0 ){
            if(power_map[recipes[i].dish]==1)
                draw.draw(*sprite_unlock, pos+glm::vec2(96, 0));
            else
                draw.draw(*sprite_jump, pos+glm::vec2(96, 0));                
        }
        else{
            for (int k = health_map[recipes[i].dish]; k > 0; --k) 
                draw.draw(*sprite_health_box, pos+glm::vec2(96, 0)+glm::vec2(43-k*4, 2), 0.4);
        }
    }
}
