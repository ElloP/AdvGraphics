#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>


struct Particle {
	float lifetime;
	float life_length;
	glm::vec3 velocity;
	glm::vec3 pos;
};

class ParticleSystem {
 public:	
	explicit ParticleSystem(){};

	explicit ParticleSystem(int size, GLuint* _shader, GLuint _texture) : max_size(size), shader(_shader), texture(_texture) {
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * max_size, nullptr, GL_STATIC_DRAW);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
		glEnableVertexAttribArray(0);
	
	};
	~ParticleSystem() {}
	
	void kill(int id);
	void spawn(Particle particle);
	void spawn();
	void update(float dt);
	void process_particles(float dt);
	void draw(glm::mat4 view);

private:
	GLuint vao;
	GLuint vbo;
	GLuint* shader;
	GLuint texture;
	float time = 0;
	float interval;
	std::vector<Particle> particles;
	int max_size;
};
