#pragma once

#include <cstdlib>

#ifndef M_PI
#	define M_PI 3.14159265358979323846f
#endif 

class mathhelper {
public:
	static float uniform_randf(const float from, const float to) {
		return from + (to - from) * float(rand()) / float(RAND_MAX);
	}

	static float randf() {
		return float(rand()) / float(RAND_MAX);
	}
};