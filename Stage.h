#pragma once

#include <string>
#include <glm/glm.hpp>

#include "Utils.h"

struct Transform2D {
	glm::vec2 Pos;
	// add rotation later

};

class Entity2D {
	std::string TextureName;
	Utils::Rect Transform;
};

class Stage {
	
public:
	Stage();
};

