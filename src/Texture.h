#ifndef TEXTURE_H
#define TEXTURE_H

#include "glad/glad.h"
class Texture
{
public:
	unsigned char id;

	explicit Texture() {};
	Texture(const char* path);
	Texture(const char* path, unsigned int _id);

	void bind();
	void unbind();
private:
	GLuint texture;
	void initTexture(const char* path);
};

#endif //!TEXTURE_H