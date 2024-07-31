#pragma once

#include <string>
#include <vector>

enum ShaderDataType
{
	Float,
	Float2,
	Float3,
	Mat3,
	Mat4
};

struct BufferElement
{
	std::string name;
	ShaderDataType type;
	unsigned size;
	unsigned offset;

	BufferElement(std::string name, ShaderDataType type);
};

class BufferLayout
{
public:
	using iterator = std::vector<BufferElement>::iterator;
	using const_iterator = std::vector<BufferElement>::const_iterator;

	BufferLayout() = default;

	BufferLayout(std::initializer_list<BufferElement> elements);

	iterator begin() { return elements.begin(); }
	iterator end() { return elements.end(); }

	const_iterator begin() const { return elements.begin(); }
	const_iterator end() const { return elements.end(); }

	unsigned GetStride() const { return stride; }

private:
	std::vector<BufferElement> elements;
	unsigned stride;
};


unsigned SizeOfType(ShaderDataType type);
unsigned GetComponentCount(ShaderDataType type);
unsigned GetGLType(ShaderDataType type);