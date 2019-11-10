#pragma once

#include "GL.hpp"
#include <glm/glm.hpp>

enum chef_state {
    Left_stand, Left_walk1, Left_walk2, Left_jump,
    Right_stand, Right_walk1, Right_walk2, Right_jump
};

class Chef {
public:
    Chef() {}
	virtual ~Chef() {}
    void set_pos(int x, int y) {
        position = glm::vec2(x, y);
    }

    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
    glm::vec2 radius = glm::vec2(30.0f, 60.0f);
    chef_state state = Right_stand;
    int health = 10;
};