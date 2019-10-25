#pragma once

#include "Part.hpp"

class Empty : public Part {
public:
    Empty() {}
	virtual ~Empty() {}

    part_type id() {
        return part_empty_type;
    }
};