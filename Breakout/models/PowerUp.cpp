#include "PowerUp.h"

PowerUp::PowerUp()
{
	position = { 0.0f, 0.0f, 0.0f };
	scale = { 0.3f, 0.3f, 0.3f };
	colour = { 1.0f, 1.0f, 1.0f };
	velocity = { 0.0f, -4.0f };
	rotation = 0.0f;

	type = PowerUpType::WidePaddle;
	active = true;
}
