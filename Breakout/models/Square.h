#pragma once

#include <glad/glad.h>

class Square
{
public:
	Square();

	void SetBuffers();

	void Render() const;

	GLfloat vertices[20] =
	{
		// vertices					texture coords
		1.0f,  1.0f, 0.0f,			1.0f, 0.0f,		// top right
		1.0f,  0.0f, 0.0f,			1.0f, 1.0f,		// bottom right
		0.0f,  0.0f, 0.0f,			0.0f, 1.0f,		// bottom left
		0.0f,  1.0f, 0.0f,			0.0f, 0.0f		// top left 	
	};

	GLuint indices[6] =
	{
		0, 1, 3,
		1, 2, 3
	};

	GLuint VBO;

	GLuint VAO;

	GLuint EBO;
};
