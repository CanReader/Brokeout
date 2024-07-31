#pragma once

class IndexBuffer
{
public:
	IndexBuffer(unsigned* data, int size);

	void Bind();
	void Unbind();
	int GetCount() const { return _count; }

private:
	unsigned int id;
	int _count;
};