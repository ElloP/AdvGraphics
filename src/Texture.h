#ifndef TEXTURE_H
#define TEXTURE_H

#include "glad/glad.h"
class Texture
{
public:
	unsigned char id;

	Texture(char* path);

	void bind();
	void unbind();
private:
	GLuint texture;
	void initTexture(char* path);
};

#endif //!TEXTURE_H