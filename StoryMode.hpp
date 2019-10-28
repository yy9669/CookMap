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

	//------ constants ---------
	const std::vector<glm::vec2> backpack_pos = {{360, 710}, {415, 710}, 
		{470, 710}, {525, 710}, {580, 710}};
	const std::vector<glm::vec2> dishes_pos = {{740, 710}, {795, 710}};


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
	bool dish_drag = false;
	glm::vec2 dish_drag_pos = glm::vec2(0,0);

	//------ background music -------
	std::shared_ptr< Sound::PlayingSample > background_music;
};
