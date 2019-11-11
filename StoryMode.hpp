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
#include <map>

struct StoryMode : Mode {
	StoryMode();
	virtual ~StoryMode();

	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//called to create menu for current scene:
	void enter_scene(float elapsed);
	bool collision(glm::vec2 pos1, glm::vec2 radius1, glm::vec2 pos2, glm::vec2 radius2);
	void resolve_collision(glm::vec2 &position, glm::vec2 radius, 
    	glm::vec2 box, glm::vec2 box_radius, glm::vec2 &velocity);
	void draw_instruction(DrawSprites& draw);
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
    float pot_time_left = 0.f;
    dish_type pot_dish;

    std::unordered_map<ingredient_type, Sprite> ingredient_map;
    std::unordered_map<dish_type, Sprite> dish_map;

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
	bool winning = false, lose = false;
	dish_type cooking_dish;		// Curently cooked dish
	bool dish_drag = false, ingre_drag = false, drag_from_backpack;
	dish_type dragged_dish;		// Curently dragged dish
	bool show_instruction = true, show_recipe = false, show_pot = false;

	glm::vec2 dish_drag_pos = glm::vec2(0,0);
	glm::vec2 ingre_drag_pos = glm::vec2(0,0);
    	ingredient_type dragging_ingre_type;

    	//-----draw position ----
    	int draw_width=768;
    	int draw_length=1024;
    	glm::vec2 health_pos = glm::vec2(68.0f, 724.0f);
    	int item_size=48;
    	int item_inteval=55;

    	float item_num=5;
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



	//------ background music -------
	std::shared_ptr< Sound::PlayingSample > background_music;



};
