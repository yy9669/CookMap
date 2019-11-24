#pragma once

#include "GL.hpp"
#include "Ingredient.hpp"
#include <glm/glm.hpp>
#include <vector>

#include <string>

enum dish_type {
    Dish0, Dish1, Dish2, Dish3, Dish4, Dish5, Dish6, Dish7, Dish8, Dish9
};

struct Dish {
public:
    Dish(dish_type type_) {
        type = type_;
    }
	virtual ~Dish() {}

    std::string name;
    dish_type type;
};