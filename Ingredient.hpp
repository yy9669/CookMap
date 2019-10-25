#pragma once

#include "Part.hpp"

class Ingredient : Part {
    Ingredient();
	virtual ~Ingredient();

    enum ingredient_type {
		Tomato,
		Potato,
        Onion
	};

    ingredient_type type = Tomato;
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 radius = glm::vec2(48.0f, 48.0f);
    
    bool obtained = false;

    part_type id() {
        return ingredient_type;
    }
};