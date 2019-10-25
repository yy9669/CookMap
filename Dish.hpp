#pragma once

#include "GL.hpp"
#include "Ingredient.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Dish {
public:
    Dish() {}
	virtual ~Dish() {}

    std::string name;
    std::vector< Ingredient > recipe;
    int restore;
};