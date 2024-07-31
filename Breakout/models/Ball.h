#pragma once

#include <glm/vec3.hpp>

#include "Model.h"
#include "../Texture.h"

class Ball : public Model
{
public:
	Ball();

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec2 velocity = { 5.5f, 10.0f };
	glm::vec3 colour;

	Texture texture;
};
