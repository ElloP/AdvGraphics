#include "Shader.h"
#include <iostream>
#include <fstream>


//using a part of labhelper and learnopengl.com implementations to load shaders
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::ifstream vs_file(vertexPath);
	std::string vs_src((std::istreambuf_iterator<char>(vs_file)), std::istreambuf_iterator<char>());

	std::ifstream fs_file(fragmentPath);
	std::string fs_src((std::istreambuf_iterator<char>(fs_file)), std::istreambuf_iterator<char>());

	const char *vs = vs_src.c_str();
	const char *fs = fs_src.c_str();

	glShaderSource(vShader, 1, &vs, nullptr);
	glShaderSource(fShader, 1, &fs, nullptr);

	unsigned int vertex, fragment;
	int success;
	char infoLog[512];
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vs, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// shader Program
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	checkCompileErrors(program, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if(type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

// int
void Shader::setUniform(char * str, int i) 
{
	glUniform1i(glGetUniformLocation(program, str), i);
}
// float
void Shader::setUniform(char * str, float f)
{
	glUniform1f(glGetUniformLocation(program, str), f);
}
// vec2
void Shader::setUniform(char * str, glm::vec2 &v) 
{
	glUniform2fv(glGetUniformLocation(program, str), 1, &v.x);
}
// vec3
void Shader::setUniform(char * str, glm::vec3 &v)
{
	glUniform3fv(glGetUniformLocation(program, str), 1, &v.x);
}
// vec4
void Shader::setUniform(char * str, glm::vec4 &v)
{
	glUniform4fv(glGetUniformLocation(program, str), 1, &v.x);
}
void Shader::setUniform(char * str, glm::mat3 &m)
{
	glUniformMatrix3fv(glGetUniformLocation(program, str), 1, false, &m[0].x);
}
void Shader::setUniform(char * str, glm::mat4 &m)
{
	glUniformMatrix4fv(glGetUniformLocation(program, str), 1, false, &m[0].x);
}