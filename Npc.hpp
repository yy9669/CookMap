#pragma once

#include "GL.hpp"
#include "Dish.hpp"
#include <glm/glm.hpp>

enum npc_type {
    npc0, npc1, npc2, npc3, npc4, npc_total
};

class Npc {
public:
    Npc() {}
    Npc(npc_type type_, float x, float y, dish_type npc_dish_) {
        type = type_;
        position = glm::vec2(x, y);
        init_position = glm::vec2(x, y);
        favorates = {npc_dish_};
        attack = 2 * (int)type + 2;
        if (type == npc4) {
            velocity = glm::vec2(-90.0f, 60.0f);
        }
    }
	virtual ~Npc() {}

    npc_type type;
    glm::vec2 position, init_position;

    glm::vec2 radius = glm::vec2(90.0f, 135.0f);
    glm::vec2 velocity = glm::vec2(50.0f, 60.0f);
    std::vector<dish_type> favorates;
    int attack;
    bool eat = false;
    bool charge = false;
};