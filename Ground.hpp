#pragma once

#include "Part.hpp"

class Ground : Part {
    Ground();
	virtual ~Ground();

    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 radius = glm::vec2(48.0f, 48.0f);

    part_type id() {
        return ground_type;
    }
};