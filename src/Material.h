#include "glm/glm.hpp"
class Material
{
public:
	glm::vec3 ambient, diffuse, specular;
	float shininess;

	void gold()
	{
		ambient = glm::vec3(0.24725, 0.1995, 0.0745);
		diffuse = glm::vec3(0.75164, 0.60648, 0.22648);
		specular = glm::vec3(0.628281, 0.555802, 0.366065);
		shininess = 0.4f;
	}
};