#pragma once

#include "BufferLayout.h"

class VertexBuffer
{
public:
	VertexBuffer(void* data, int size);

	void Bind();
	void Unbind();

	void SetLayout(const BufferLayout& layout)
	{
		this->layout = layout;
	}

	const BufferLayout& GetLayout()
	{
		return layout;
	}

private:
	unsigned int id;

	BufferLayout layout;
};