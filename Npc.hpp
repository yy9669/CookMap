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
        init_position = glm::vec2(x, y);
    }
	virtual ~Npc() {}

    npc_type type;
    glm::vec2 position, init_position;
    glm::vec2 radius = glm::vec2(80.0f, 120.0f);
    glm::vec2 velocity = glm::vec2(0.0f, 60.0f);
    bool eat = false;
};