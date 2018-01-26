#ifndef ENVIRONMENT_MAP_H
#define ENVIRONMENT_MAP_H

#include "glm/glm.hpp"
#include "Shader.h"

class EnvironmentMap 
{
public:
	EnvironmentMap(char *faces[6]);
	EnvironmentMap(char *faceX, char *facenegX, char *faceY, char *facenegY, char *faceZ, char *facenegZ);
	void bind();
	void unbind();
	void draw(const glm::mat4 &view, const glm::mat4 &projection);
private:
	unsigned int id;
	Shader shader;
};
#endif //ENVIRONMENT_MAP_H!!!!!
