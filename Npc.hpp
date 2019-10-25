#pragma once

#include "GL.hpp"
#include <glm/glm.hpp>

enum npc_type {
    npc0, npc1, npc2, npc3
};

class Npc {
public:
    Npc(npc_type type_, float x, float y) {
        type = type_;
        position = glm::vec2(x, y);
        radius = glm::vec2(48.0f, 48.0f);
    }
	virtual ~Npc() {}

    npc_type type;
    glm::vec2 position, radius, velocity = glm::vec2(0.0f, 0.0f);
};