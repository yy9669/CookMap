#pragma once

#include "GL.hpp"
#include <glm/glm.hpp>

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
    int health = 10;
};