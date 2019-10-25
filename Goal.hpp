#pragma once

#include "Part.hpp"

class Goal : public Part {
public:
    Goal() {}
	virtual ~Goal() {}

    part_type id() {
        return part_goal_type;
    }
};