#include "ParticleSystem.h"
#include <algorithm>


void ParticleSystem::kill(int id) {
	particles[id] = particles[particles.size() - 1];
	particles.pop_back();
}

void ParticleSystem::spawn(Particle particle) {
	if (particles.size() < max_size) {
		particles.push_back(particle);
	}
}

void ParticleSystem::process_particles(float dt) {
	for (unsigned i = 0; i < particles.size(); ++i) {
		if (particles[i].lifetime > particles[i].life_length) {
			kill(i);
			i--;
		}
	}
	for (unsigned i = 0; i < particles.size(); ++i) {
		particles[i].pos += particles[i].velocity * dt;
		particles[i].lifetime += dt;
	}
}

void ParticleSystem::draw(glm::mat4 view) {
	texture->bind();
	shader->use();

	glEnable(GL_PROGRAM_POINT_SIZE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	unsigned int active_particles = particles.size();

	if (active_particles == 0) return;

	std::vector<glm::vec4> data;

	for each (Particle particle in particles)
	{
		data.push_back(glm::vec4(glm::vec3(view * glm::vec4(particle.pos, 1.0f)), particle.lifetime));
	}

	std::sort(data.begin(), std::next(data.begin(), active_particles),
		[](const glm::vec4 &lhs, const glm::vec4 &rhs) { return lhs.z < rhs.z; });


	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * active_particles, &data[0u]);

	glBindVertexArray(vao);

	glDrawArrays(GL_POINTS, 0, active_particles);

	glDisable(GL_BLEND);
	glDisable(GL_PROGRAM_POINT_SIZE);
	//glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
}
