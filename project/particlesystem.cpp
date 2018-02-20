#include "ParticleSystem.h"
#include <algorithm>
#include <labhelper.h>



void ParticleSystem::run(glm::mat4 view, float dt) {
	update(dt);
	spawn();
	draw(view);
	process_particles(dt);	
}

void ParticleSystem::update(float dt) {
	time += dt;
}

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

void ParticleSystem::spawn() {
	interval = 0.1f;
	if (time > interval) {
		time -= interval;
		for (int i = 0; i < 10; i++) {
			const float theta = labhelper::uniform_randf(0.f, 2.f * M_PI);
			const float u = labhelper::uniform_randf(-1.0f, 1.f);
			//const float u = mathhelper::uniform_randf(0.95f, 1.f);
			glm::vec3 pos = glm::vec3(sqrt(1.f - u * u) * cosf(theta), u, sqrt(1.f - u * u) * sinf(theta));
			Particle particle;
			particle.velocity = pos * 1.0f;
			particle.pos = pos * 1.0f;
			particle.life_length = 3;
			particle.lifetime = 0;
			spawn(particle);
		}
	}
}

void ParticleSystem::draw(glm::mat4 view) {
	glUseProgram(shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	
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

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
}
