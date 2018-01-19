#ifndef MESH_H
#define MESH_H
#include "Vertex.h"
#include <vector>

class Mesh
{
public:
	void bind();
	void unbind();
	Mesh(std::vector<Vertex> vertices);
	~Mesh();
private:
	unsigned int vao;
};

#endif // MESH_H!!!
