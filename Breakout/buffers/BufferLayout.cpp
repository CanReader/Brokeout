#include "BufferLayout.h"

#include <glad/glad.h>

BufferElement::BufferElement(std::string name, ShaderDataType type)
	: name(name), type(type), size(SizeOfType(type))
{

}

BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements)
	: elements(elements)
{
	unsigned offset = 0;
	stride = 0;

	for (auto& element : this->elements)
	{
		element.offset = offset;
		offset += element.size;
		stride += element.size;
	}
}

unsigned SizeOfType(ShaderDataType type)
{
	switch (type)
	{
	case Float:
		return sizeof(float);

	case Float2:
		return 2 * sizeof(float);

	case Float3:
		return 3 * sizeof(float);

	case Mat3:
		return 3 * 3 * sizeof(float);

	case Mat4:
		return 4 * 4 * sizeof(float);
	}

	return 0;
}

unsigned GetComponentCount(ShaderDataType type)
{
	switch (type)
	{
	case Float:
		return 1;

	case Float2:
		return 2;

	case Float3:
		return 3;

	case Mat3:
		return 3 * 3;

	case Mat4:
		return 4 * 4;
	}

	return 0;
}

unsigned GetGLType(ShaderDataType type)
{
	switch (type)
	{
	case Float:
	case Float2:
	case Float3:
	case Mat3:
	case Mat4:
		return GL_FLOAT;
	}

	return 0;
}