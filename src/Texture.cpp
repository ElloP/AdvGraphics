#include "Texture.h"
#include "stb_image.h"
#include <iostream>

Texture::Texture(const char* path)
{
	initTexture(path);
}

void Texture::bind()
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::initTexture(const char* path) 
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);


	unsigned char* data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
	if(data == nullptr)
	{
		std::cerr << "Failed to initalize texture::" << path << std::endl;
	}

	glGenTextures(1, &texture);

	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	unbind();

	stbi_image_free(data);
}