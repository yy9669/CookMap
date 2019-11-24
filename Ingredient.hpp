#pragma once

#include "Part.hpp"

enum ingredient_type {
    Item1,
    Item2,
    Item3,
    Item4,
    Item5,
    Item6,
    Item7,
    Item8,
    Item9,
    Item10,
    Item11,
    Item12,
    Item13,
    Item14,
    Item15,
    Item16,
    Item17,
    Item18,
    Item19,
    Item20,
    Item21,
    Item22
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
    float grow_count = 0;
};