#pragma once
/*
 * StoryMode implements a story about The Planet of Choices.
 *
 */

#include "Mode.hpp"
#include "Sound.hpp"
#include "Part.hpp"
#include "Empty.hpp"
#include "Chef.hpp"
#include "Ground.hpp"
#include "Goal.hpp"
#include "Npc.hpp"
#include "Ingredient.hpp"
#include "Dish.hpp"
#include "DrawSprites.hpp"
#include "Sprite.hpp"
#include <string>
#include <map>
#include <vector>

typedef struct {
    std::vector<ingredient_type> ingredients;
    std::vector<bool> show;
    dish_type dish;
    int cost;
} Recipe;

struct StoryMode : Mode {
	StoryMode();
	virtual ~StoryMode();

	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	void restart(StoryMode* mode);
	//called to create menu for current scene:
	void enter_scene(float elapsed);

	enum {
		mainmenu,
		website,
		credit,
		gamescene
	} location = mainmenu;


	bool collision(glm::vec2 pos1, glm::vec2 radius1, glm::vec2 pos2, glm::vec2 radius2);
	void resolve_collision(glm::vec2 &position, glm::vec2 radius, 
    	glm::vec2 box, glm::vec2 box_radius, glm::vec2 &velocity);
	void draw_instruction(DrawSprites& draw);
	void draw_tmp_instruction(DrawSprites& draw);
	void draw_recipe(DrawSprites& draw);
    void draw_pot(DrawSprites& draw);

	//------ constants ---------
	const std::vector<glm::vec2> backpack_pos = {{335, 710}, {390, 710}, 
		{445, 710}, {500, 710}, {555, 710}};
	const std::vector<glm::vec2> dishes_pos = {{670, 710}, {725, 710}};


	//------ story state -------
	enum {
		Walking,
		Cooking
	} game_mode = Walking;
	
	std::vector<std::vector<Part*>> parts;
	std::vector<Npc*> npcs;
	
	Chef player;
	std::vector<ingredient_type> backpack;
	std::vector<dish_type> dishes;
    std::vector<ingredient_type> pots;
    float pot_time_left = 0;
    dish_type pot_dish;
    float timepoint = 0.f;
	float curt_time, last_time, power_time;

    std::unordered_map<ingredient_type, Sprite> ingredient_map;
    std::unordered_map<dish_type, Sprite> dish_map;
    std::unordered_map<dish_type, int> health_map=
    {{Dish1,5},{Dish2,3},{Dish3,2},{Dish4,4}, {Dish5,4},  {Dish6,5}, {Dish7, 3}, {Dish8, 6}, {Dish9, 6}, {Dish0,-1}};
	// 1 => unlock a recipe, 2 => big jump
	std::unordered_map<dish_type, int> power_map={{Dish1,1}, {Dish2,2}, {Dish5, 2}, {Dish6, 1}, {Dish8,1}, {Dish9, 2}};
	// std::unordered_map<dish_type, int> npc_map=
	//Current control signals:
	struct {
		bool left = false;
		bool right = false;
		bool up = false;
		bool down = false;
	} controls;

	glm::vec2 view_min = glm::vec2(0,0);
	glm::vec2 view_max = glm::vec2(1024, 768);

	bool proto_cook = false;
	bool winning = false, lose = false, winned=false;
	float jump_interval = 0.f;
	bool jumping = false;

	std::vector<Recipe> recipes = {
		{{Item11, Item2}, {true, true}, Dish2, 5},  // bread, sausage => hot dog
	    {{Item4, Item5, Item8}, {true, true, true}, Dish3, 5},  // apple, dragon fruit,tamato => juice
        {{Item6, Item7, Item9}, {true, true, false}, Dish3, 5},  // grape, orange, kiwi => juice
        {{Item1, Item2, Item3}, {true, true, false}, Dish1, 5},  // flour, sausage, pepper => pizza
	    {{Item12, Item15, Item14}, {true, true, true}, Dish4, 5},  // broccoli, egg, avocado => salad
	    {{Item12, Item15, Item8}, {true, true, false}, Dish4, 5},  // broccoli, egg, tomato => salad
	    {{Item16, Item17, Item3}, {true, true, false}, Dish5, 5},  // twig, meat, pepper => bbq
	    {{Item18, Item17, Item19}, {true, false, false}, Dish6, 5},  // mushroom, meat, onion => steak
	    {{Item19, Item21}, {true, false}, Dish7, 5},  // onion, water => onion soup
	    {{Item21, Item20, Item10}, {true, false, false}, Dish8, 5},  // water, fish, lemon => steamed fish
	    {{Item15, Item22, Item7}, {false, false, false}, Dish9, 5},  // egg, milk, orange  => orange ice cream
	};
	Recipe *cooking_recipe;   // Currently cooked recipe
	dish_type cooking_dish;   // Currently cooked recipe

	std::unordered_map<ingredient_type, int> ingre_cost = {
        {Item1, 1},  // flour
        {Item2, 2},  // sausage
        {Item3, 1},  // pepper
        {Item4, 0},  // apple
        {Item5, 0},  // dragon fruit
        {Item6, 0},  // grape
        {Item7, 0},  // orange
        {Item8, 0},  // tomato
        {Item9, 0},  // kiwi
        {Item10, 1},  // lemon
        {Item11, 1},  // bread
        {Item12, 1},  // broccoli
        {Item13, 1},  // almond
        {Item14, 0},  // avocado
        {Item15, 1},  // egg
        {Item16, 1},  // twig
        {Item17, 2},  // meat
        {Item18, 1},  // mushroom
        {Item19, 0},  // onion
        {Item20, 2},  // fish
        {Item21, 0},  // water
        {Item22, 3},  // milk
	};

	bool dish_drag = false, ingre_drag = false, drag_from_backpack;
	dish_type dragged_dish;		// Currently dragged dish
	bool show_instruction = true, show_recipe = false, show_pot = false;
	bool NPC_instruction = false;
	bool COOK_instruction = false;
	float tmp_instruction_time = 0.0f;
	std::string tmp_instruction;


	glm::vec2 dish_drag_pos = glm::vec2(0,0);
	glm::vec2 ingre_drag_pos = glm::vec2(0,0);
	glm::vec2 drag_offset  = glm::vec2(0,0);
    	ingredient_type dragging_ingre_type;

    	//-----draw position ----
    	int draw_width=768;
    	int draw_length=1024;
    	glm::vec2 health_pos = glm::vec2(68.0f, 724.0f);
    	int item_size=48;
    	int item_inteval=55;

        int item_num=5;
    	int item_start_x=335;
    	int item_start_y=10;

    	int dish_num=2;
    	int dish_start_x=670;
    	int dish_start_y=10;

    	int pot_x=845;
    	int pot_y=10;
    	int recipe_x=785;
    	int recipe_y=10;

    	int garbage_x=905;
    	int garbage_y=10;
    	int help_x=965;
    	int help_y=10;
    	float stealcd=0;

    int scene_num = 0, scene_target = 0;
    float scene_transition = 10.f;
    int restarting = 0;

	// backup state
	Chef player_b;
	std::vector<ingredient_type> backpack_b;
	std::vector<dish_type> dishes_b;
    std::vector<ingredient_type> pots_b;
	std::unordered_map<dish_type, int> power_map_b={{Dish1,1},{Dish2,2}};
	std::unordered_map<Npc*, Npc> npcs_b;
	void save_state(StoryMode* mode);
	void load_state(StoryMode* mode);

	//------ background music -------
	std::shared_ptr< Sound::PlayingSample > background_music;



};
