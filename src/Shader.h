#ifndef SHADER_H
#define SHADER_H


//TODO: add functions to set uniforms
#include "glad/glad.h"
#include <string>
#include <glm\glm.hpp>


class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	void setUniform(char * str, int i);
	void setUniform(char * str, float f);
	void setUniform(char * str, glm::vec2 &v);
	void setUniform(char * str, glm::vec3 &v);
	void setUniform(char * str, glm::vec4 &v);
	inline int getProgram() { return program; }
private:
	GLuint program;
	void checkCompileErrors(unsigned int shader, std::string type);
};

#endif //!SHADER_H