#pragma once

#include "GL.hpp"
#include <glm/glm.hpp>

class Part {
    Part();
	virtual ~Part();
    enum part_type {
        npc_type,
        ingredient_type,
        ground_type,
        empty_type
    };
    glm::vec2 position;
    glm::vec2 radius;
    virtual part_type id() = 0;
};