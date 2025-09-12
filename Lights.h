#pragma once

#include <glm/glm.hpp>

struct PointLight {

	glm::vec3 Pos;
	glm::vec3 Color;

	/*float Constant;
	float Linear;
	float Quadratic;
	
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;*/
};

struct DirLight {

	glm::vec3 Dir;

	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
};