
/*
 * StoryMode implements a story about The Planet of Choices.
 *
 */

#include "Mode.hpp"

struct StoryMode : Mode {
	StoryMode();
	virtual ~StoryMode();

	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//called to create menu for current scene:
	void enter_scene();

	//------ story state -------
	enum {
		Dunes,
		Oasis,
		Hill
	} location = Dunes;
	bool have_stone = false;
	bool added_stone = false;
	struct {
		bool first_visit = true;
		bool wont_leave = false;
	} dunes;
	struct {
		bool first_visit = true;
		bool took_stone = false;
	} oasis;
	struct {
		bool first_visit = true;
		bool added_stone = false;
	} hill;
	
	glm::vec2 view_min = glm::vec2(0,0);
	glm::vec2 view_max = glm::vec2(256, 224);
};
