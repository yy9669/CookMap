#pragma once

#include "GL.hpp"
#include <glm/glm.hpp>


enum part_type {
    part_ingredient_type,
    part_ground_type,
    part_empty_type,
    part_goal_type
};

class Part {
public:
    Part() {}
	virtual ~Part() {}
    virtual part_type id() = 0;
    void set_pos(int x, int y) {
        position = glm::vec2(48.0f*x, 48.0f*y);
    }

    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 radius = glm::vec2(24.0f, 24.0f);
};