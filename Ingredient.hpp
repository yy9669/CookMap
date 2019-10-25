#pragma once

#include "Part.hpp"

enum ingredient_type {
    Tomato,
    Potato,
    Onion
};

class Ingredient : public Part {
public:
    Ingredient(ingredient_type type_) {
        type = type_;
    }
	virtual ~Ingredient() {}
    part_type id() {
        return part_ingredient_type;
    }

    ingredient_type type;
    bool obtained = false;
};