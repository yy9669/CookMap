#pragma once

#include "GL.hpp"
#include "Ingredient.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>

enum dish_type {
    Dish1, Dish2, Dish3
};

struct Dish {
public:
    Dish(dish_type type_) {
        type = type_;
    }
	virtual ~Dish() {}

    std::string name;
    std::vector< Ingredient > recipe;
    dish_type type;
    int restore;
};