#pragma once

#include "Part.hpp"

class Ground : public Part {
public:
    Ground(char map_char) {
    	g_type = map_char;
    }
	virtual ~Ground() {}

    part_type id() {
        return part_ground_type;
    }

    char g_tpye() {
    	return g_type;
    }

    char g_type;
};