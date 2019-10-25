#pragma once

#include "GL.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Dish {
    Dish();
	virtual ~Dish();

    std::string name;
    std::vector< Ingredient > recipe;
    int health;
};