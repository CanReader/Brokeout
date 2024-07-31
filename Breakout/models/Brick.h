#pragma once

#include <glm/vec3.hpp>

#include "Model.h"
#include "../Texture.h"

class Brick : public Model
{
public:
	Brick();
	
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 colour;
	float rotation;

	Texture texture;
	Texture cracked;

	int hits;
	
	bool brickAlive;
	bool brickDying;
};
