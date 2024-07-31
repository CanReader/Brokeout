#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>

#include "../Vertex.h"

class Model
{
public:
	void loadASSIMP(std::string const& path);

	void render();

	void setBuffers();

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

private:
	GLuint VBO;

	GLuint VAO;

	GLuint EBO;
};