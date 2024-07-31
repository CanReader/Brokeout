#pragma once

#include "Model.h"
#include "../Texture.h"

class GameObject : public Model
{
public:
	GameObject();

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 colour;

	Texture texture;

	float rotation;
	
	bool active;
};
