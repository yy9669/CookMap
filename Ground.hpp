#pragma once

#include "Part.hpp"

class Ground : public Part {
public:
    Ground() {}
	virtual ~Ground() {}

    part_type id() {
        return part_ground_type;
    }
};