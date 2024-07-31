#pragma once

#include <glad/glad.h>
#include <glm/vec3.hpp>

#include "Model.h"
#include "../Texture.h"

class Player : public Model
{
public:
	Player();

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 colour;
	glm::vec2 velocity = {15.0f,0.0f};

	Texture texture;

	float rotation;
	
	int lives;
	int score;
};
