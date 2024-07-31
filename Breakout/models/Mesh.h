#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Shader.h"

#include <string>
#include <vector>


#include "../Vertex.h"
#include "../VertexArray.h"

struct TextureInfo
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	// constructor
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureInfo> textures)
	{
		this->_textures = textures;

		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh(vertices, indices);
	}

	// render the mesh
	void Draw(Shader& shader)
	{
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;

		for (unsigned int i = 0; i < _textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding

			// retrieve texture number (the N in diffuse_textureN)
			std::string number;
			std::string name = _textures[i].type;
			
			if (name == "texture_diffuse")
			{
				number = std::to_string(diffuseNr++);			
			}
			else if (name == "texture_specular")
			{
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			}
			else if (name == "texture_normal")
			{
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			}
			else if (name == "texture_height")
			{
				number = std::to_string(heightNr++); // transfer unsigned int to stream
			}

			// now set the sampler to the correct texture unit
			shader.setInt(name + number, i);

			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, _textures[i].id);
		}

		// draw mesh
		_vao->Bind();
		glDrawElements(GL_TRIANGLES, _vao->GetIBO()->GetCount(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

private:
	std::vector<TextureInfo> _textures;
	std::unique_ptr<VertexArray> _vao;

	void setupMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
	{
		_vao = std::make_unique<VertexArray>();
		_vao->Bind();

		auto vbo = std::make_unique<VertexBuffer>(&vertices[0], vertices.size() * sizeof(Vertex));
		vbo->Bind();

		vbo->SetLayout(
			{
				{"aPos", Float3},
				{"aNorm", Float3},
				{"aTexCoords", Float2},
				{"aTangent", Float3},
				{"aBitangent", Float3}
			}
		);

		auto ibo = std::make_unique<IndexBuffer>(&indices[0], indices.size());
		ibo->Bind();

		_vao->SetVertexBuffer(std::move(vbo));
		_vao->SetIndexBuffer(std::move(ibo));
	}
};
#endif