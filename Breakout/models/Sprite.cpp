#include "Sprite.h"

Sprite::Sprite()
{
	position = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 0.0f };
	colour = { 1.0f, 1.0f, 1.0f };

	active = true;
}