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
#include <map>

struct StoryMode : Mode {
	StoryMode();
	virtual ~StoryMode();

	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//called to create menu for current scene:
	void enter_scene(float elapsed);

	//------ story state -------
	enum {
		Walking,
		Cooking
	} game_mode = Walking;
	
	std::vector<std::vector<Part*>> parts;
	std::vector<Npc*> npcs;
	
	Chef player;
	std::map< Ingredient, int > backpack;
	std::map< Dish, int > dishes;

	//Current control signals:
	struct {
		bool left = false;
		bool right = false;
		bool up = false;
		bool down = false;
	} controls;

	glm::vec2 view_min = glm::vec2(0,0);
	glm::vec2 view_max = glm::vec2(1024, 768);

	//------ background music -------
	std::shared_ptr< Sound::PlayingSample > background_music;
};
