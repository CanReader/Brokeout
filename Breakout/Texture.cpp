#include "Texture.h"

#include <iostream>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

unsigned int Texture::Load(const std::string& fileName)
{
    // generate and bind the textures
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int nrChannels;
    unsigned char* data = stbi_load(fileName.c_str(), &_width, &_height, &nrChannels, 0);

    if (data)
    {
        GLenum format = GL_RGB;

        if (nrChannels == 1)
        {
            format = GL_RED;
        }
        else if (nrChannels == 3)
        {
            format = GL_RGB;
        }
        else if (nrChannels == 4)
        {
            format = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    return _texture;
}

void Texture::Bind(unsigned textureSlot) const
{
    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(GL_TEXTURE_2D, _texture);
}
