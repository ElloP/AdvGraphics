#ifndef SHADER_H
#define SHADER_H


//TODO: add functions to set uniforms
#include "glad/glad.h"
#include <string>

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);

	inline int getProgram() { return program; }
private:
	GLuint program;
	void checkCompileErrors(unsigned int shader, std::string type);
};

#endif //!SHADER_H