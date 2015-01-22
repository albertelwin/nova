
#include <GL/glfw3.h>

#include <cstdint>
#include <cstdio>

#include <math.hpp>

#include <math.cpp>

float get_current_time() {
	return static_cast<float>(glfwGetTime());
}

namespace math {
	Vec2 operator+(Vec2 const & x, Vec2 const & y) {
		Vec2 tmp = x;
		tmp.x += y.x;
		tmp.y += y.y;
		return tmp;
	}
}

int main() {
	if(!glfwInit()) {
		return 0;
	}

	math::Vec2 v0 = { 2.f, 2.f };
	math::Vec2 v1 = { 3.f, 3.f };

	float const start_time = get_current_time();

	//NOTE: 100m calls
	math::Vec2 v = { 0.f, 0.f };
	for(uint32_t i = 0; i < 100000000; i++) {
		//NOTE: ~0.85sec
		//v = math::vec2_add(v, v0);

		//NOTE: ~0.84sec
		//v = { v.x + v0.x, v.y + v0.y };

		//NOTE: ~0.1sec
		//v.x += v0.x;
		//v.y += v0.y;

		//NOTE: ~0.1sec
		// v.x = v.x + v0.x;
		// v.y = v.y + v0.y;

		v = v + v0;
		//v = v0 + v1;

		//v = v0 + v1;
	}

	float const total_time = get_current_time();
	std::printf("LOG: Total time: %f, result: %f, %f\n", total_time, v.x, v.y);

	return 0;
}