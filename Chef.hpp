#pragma once

#include "GL.hpp"
#include <glm/glm.hpp>

class Chef {
public:
    Chef() {}
	virtual ~Chef() {}

    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
    glm::vec2 radius = glm::vec2(48.0f, 48.0f);
    int health = 100;
};