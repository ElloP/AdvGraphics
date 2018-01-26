#include "EnvironmentMap.h"
#include "stb_image.h"
#include "glad/glad.h"
#include "FullScreenQuad.h"

EnvironmentMap::EnvironmentMap(char *faces[6]) : EnvironmentMap(faces[0], faces[1], faces[2], faces[3], faces[4], faces[5])
{}

EnvironmentMap::EnvironmentMap(char *faceX, char *facenegX, char *faceY, char *facenegY, char *faceZ, char *facenegZ)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	int width, height, nrChannels;
	unsigned char *data;
	data = stbi_load(faceX, &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	data = stbi_load(facenegX, &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	data = stbi_load(faceY, &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	data = stbi_load(facenegY, &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	data = stbi_load(faceZ, &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	data = stbi_load(facenegZ, &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	shader = Shader("C:\\Skola\\Advanced Graphics\\AdvGraphics\\src\\Shaders\\cubemap.vs", 
		"C:\\Skola\\Advanced Graphics\\AdvGraphics\\src\\Shaders\\cubemap.fs");
}

void EnvironmentMap::bind()
{
	
	glBindTexture(GL_TEXTURE_2D, id);
	glActiveTexture(GL_TEXTURE6);
	
}
void EnvironmentMap::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
void EnvironmentMap::draw(const glm::mat4 &view, const glm::mat4 &projection)
{
	glDepthMask(GL_FALSE);
	bind();
	shader.use();
	glm::mat4 inverse = glm::inverse(projection * view);
	shader.setUniform("inv_PV", inverse);
	shader.setUniform("camera_pos", glm::vec3(view[0].w,view[1].w,view[2].w));
		FullscreenQuad::drawFullscreenQuad();
	unbind();
	glDepthMask(GL_TRUE);
}