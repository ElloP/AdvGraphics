
#include "heightfield.h"

#include <iostream>
#include <stdint.h>
#include <vector>
#include <glm/glm.hpp>
#include <stb_image.h>

using namespace glm;
using std::string;

HeightField::HeightField(void)
	: m_meshResolution(0)
	, m_vao(UINT32_MAX)
	, m_positionBuffer(UINT32_MAX)
	, m_uvBuffer(UINT32_MAX)
	, m_indexBuffer(UINT32_MAX)
	, m_numIndices(0)
	, m_texid_hf(UINT32_MAX)
	, m_texid_diffuse(UINT32_MAX)
	, m_heightFieldPath("")
	, m_diffuseTexturePath("")
{
}

void HeightField::loadHeightField(const std::string &heigtFieldPath)
{
	int width, height, components;
	stbi_set_flip_vertically_on_load(true);
	float * data = stbi_loadf(heigtFieldPath.c_str(), &width, &height, &components, 1);
	if (data == nullptr) {
		std::cout << "Failed to load image: " << heigtFieldPath << ".\n";
		return;
	}

	if (m_texid_hf == UINT32_MAX) {
		glGenTextures(1, &m_texid_hf);
	}
	glBindTexture(GL_TEXTURE_2D, m_texid_hf);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data); // just one component (float)

	m_heightFieldPath = heigtFieldPath;
	std::cout << "Successfully loaded height field texture: " << heigtFieldPath << ".\n";
}

void HeightField::loadDiffuseTexture(const std::string &diffusePath)
{
	int width, height, components;
	stbi_set_flip_vertically_on_load(true);
	uint8_t * data = stbi_load(diffusePath.c_str(), &width, &height, &components, 3);
	if (data == nullptr) {
		std::cout << "Failed to load image: " << diffusePath << ".\n";
		return;
	}

	if (m_texid_diffuse == UINT32_MAX) {
		glGenTextures(1, &m_texid_diffuse);
	}

	glBindTexture(GL_TEXTURE_2D, m_texid_diffuse);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // plain RGB
	glGenerateMipmap(GL_TEXTURE_2D);

	std::cout << "Successfully loaded diffuse texture: " << diffusePath << ".\n";
}


void HeightField::generateMesh(int tesselation)
{
	// generate a mesh in range -1 to 1 in x and z
	// (y is 0 but will be altered in height field vertex shader)
	unsigned int tessplus = tesselation + 1;
	unsigned int vertCount = pow(tessplus, 2);
	unsigned int totalSize = vertCount * 5;

	std::vector<float> verts(totalSize);
	
	for (int i = 0; i < totalSize; i+=5) {
		unsigned int row = (i/5) / (tessplus);
		unsigned int col = (i/5) % (tessplus);
		float u = col / (float)tesselation;
		float v = row / (float)tesselation;
		verts[i]	 = u*2 - 1.0f; // x
		verts[i + 1] = 0.0f; // y
		verts[i + 2] = v*2 - 1.0f; // z
		verts[i + 3] = u; // u
		verts[i + 4] = v; // v
	}
	//Index data
	m_numIndices = tessplus * 2 * tesselation + tesselation;
	std::vector<unsigned int> indices(m_numIndices);
	int offset = 0;
	for (int i = 0; i < tesselation; i++) {
		for (int j = 0; j < tessplus*2; j+=2) {
			indices[i * tessplus * 2 + j + offset]		= j/2 + i*tessplus;
			indices[i * tessplus * 2 + j + 1 + offset]	= j/2 + (i+1)*tessplus;
		}
		indices[i*tessplus * 2 + tessplus * 2 + offset] = UINT_MAX;
		offset++;
	}
	
	

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &m_indexBuffer);
	
	glGenVertexArrays(1, &m_vao);
	// 1. bind Vertex Array Object
	glBindVertexArray(m_vao);
	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts.size(), verts.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numIndices * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	

}

void HeightField::submitTriangles(void)
{
	if (m_vao == UINT32_MAX) {
		std::cout << "No vertex array is generated, cannot draw anything.\n";
		return;
	}
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, m_texid_hf);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, m_texid_diffuse);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(UINT_MAX);
	glBindVertexArray(m_vao);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glDrawElements(GL_TRIANGLE_STRIP, m_numIndices, GL_UNSIGNED_INT, 0);

	glDisable(GL_PRIMITIVE_RESTART);
	
}