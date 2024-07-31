#include "VertexArray.h"

#include <glad/glad.h>

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &id);
}

void VertexArray::Bind()
{
	glBindVertexArray(id);
}

void VertexArray::Unbind()
{
	glBindVertexArray(0);
}

void VertexArray::SetVertexBuffer(std::unique_ptr<VertexBuffer> vb)
{
	Bind();
	vb->Bind();

	const auto& layout = vb->GetLayout();

	auto i = 0;

	for (const auto& element : layout)
	{
		glVertexAttribPointer(i, GetComponentCount(element.type), GetGLType(element.type),
			GL_FALSE, layout.GetStride(), (void*)element.offset);

		glEnableVertexAttribArray(i);

		++i;
	}

	vbo = std::move(vb);
}

void VertexArray::SetIndexBuffer(std::unique_ptr<IndexBuffer> ib)
{
	Bind();
	ib->Bind();

	ibo = std::move(ib);
}