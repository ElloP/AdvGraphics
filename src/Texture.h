#ifndef TEXTURE_H
#define TEXTURE_H

#include "glad/glad.h"
class Texture
{
public:
	unsigned char id;

	Texture(const char* path);

	void bind();
	void unbind();
private:
	GLuint texture;
	void initTexture(const char* path);
};

#endif //!TEXTURE_H