#pragma once
#include "ParticleSystem.h"
#include <glm/mat4x4.hpp>

class ParticleGalaxy {
public:
	void draw(const glm::mat4 view, float deltaTime);
	void add(ParticleSystem* system);
	void remove(ParticleSystem* system);

private:
	std::vector<ParticleSystem*> systems;

};
