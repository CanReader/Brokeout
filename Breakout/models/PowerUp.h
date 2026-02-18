#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Model.h"
#include "../Texture.h"

enum class PowerUpType { WidePaddle, ExtraLife, SlowBall, FastBall };

class PowerUp : public Model
{
public:
	PowerUp();

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 colour;
	glm::vec2 velocity;
	float rotation;

	PowerUpType type;
	bool active;

	Texture texture;
};
