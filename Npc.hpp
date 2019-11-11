#pragma once

#include "GL.hpp"
#include "Dish.hpp"
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
        switch (type) {
            case npc0:
                favorates = {Dish1, Dish2, Dish3};
                attack = 2;
                break;
            case npc1:
                favorates = {Dish2, Dish4};
                attack = 4;
                break;
            case npc2:
                favorates = {Dish3, Dish4};
                attack = 6;
                break;
            case npc3:
                favorates = {Dish5};
                attack = 8;
                break;
        }
    }
	virtual ~Npc() {}

    npc_type type;
    glm::vec2 position, init_position;
    glm::vec2 radius = glm::vec2(80.0f, 120.0f);
    glm::vec2 velocity = glm::vec2(0.0f, 60.0f);
    std::vector<dish_type> favorates;
    int attack;
    bool eat = false;
};