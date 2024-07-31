#pragma once

#include <glm/vec3.hpp>

#include "Square.h"
#include "../Texture.h"

class Sprite
	: public Square
{
public:
	Sprite();

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 colour;

	Texture texture;

	bool active;
};
