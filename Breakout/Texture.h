#pragma once

#include <string>

class Texture
{
public:
	unsigned int Load(const std::string& fileName);
	void Bind(unsigned textureSlot = 0) const;

	int GetWidth() const { return _width; }
	int GetHeight() const { return _height; }

	int GetTexture() const { return _texture; }

private:
	unsigned int _texture = 0;

	int _width = 0;
	int _height = 0;
};