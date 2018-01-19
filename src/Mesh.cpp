#include "Mesh.h"
#include "glad/glad.h"
#include <iostream>


void Mesh::bind() 
{
	glBindVertexArray(vao);
}
void Mesh::unbind() 
{
	glBindVertexArray(0);
}
template<size_t SIZE, class T> inline size_t array_size(T(&arr)[SIZE]) {
	return SIZE;
}
Mesh::Mesh(std::vector<Vertex> vertices)
{
	unsigned int vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unbind();
	// May have to delete buffers (?)
}
Mesh::~Mesh() {};
