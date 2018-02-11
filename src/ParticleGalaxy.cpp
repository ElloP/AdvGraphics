#pragma once
#include "ParticleGalaxy.h"

void ParticleGalaxy::draw(const glm::mat4 view, float deltaTime) {
	for (int i = 0; i < systems.size(); i++) {
		systems[i]->process_particles(deltaTime);
		systems[i]->draw(view);
	}
}


void ParticleGalaxy::add(ParticleSystem* system) {
	systems.push_back(system);
}

void ParticleGalaxy::remove(ParticleSystem* system) {
	for (int i = 0; i < systems.size(); i++) {
		if (systems[i] == system) {
			ParticleSystem* temp = systems[systems.size() - 1];
			systems[systems.size() - 1] = systems[i];
			systems[i] = temp;
			systems.pop_back();
			break;
		}
	}
}

