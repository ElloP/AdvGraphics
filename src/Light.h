#ifndef LIGHT_H
#define LIGHT_H

#include "glm/glm.hpp"

class Light
{
public:
	glm::vec3 position, ambient, diffuse, specular;


	void tutorial(glm::vec3 pos)
	{
		position = pos;
		ambient = glm::vec3(0.1f);
		diffuse = glm::vec3(0.6f);
		specular = glm::vec3(1);
	}
	void setAmbient(glm::vec3 color)
	{
		ambient = color;
	}
	void setDiffuse(glm::vec3 color)
	{
		diffuse = color;
	}
	void setSpecular(glm::vec3 color)
	{
		specular = color;
	}
};
struct DirectionalLight
{
	Light light;
};

struct PointLight
{
	Light light;
	float constant, linear, quadratic;
};
struct SpotLight
{
	Light light;
	glm::vec3 direction;
	float cutoff;
};

#endif