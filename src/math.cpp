
#include <math.hpp>

#include <cstdlib>
#include <cmath>

namespace math {
	float min_float(float x, float y) {
		return x < y ? x : y;
	}

	float max_float(float x, float y) {
		return x > y ? x : y;
	}

	float clamp_float(float x, float u, float v) {
		return (x > u) ? ((x < v) ? x : v) : u;
	}

	float frac_float(float x) {
		return x - static_cast<int32_t>(x);
	}

	float random_float() {
		return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	}

	float pseudo_random_float(float x) {
		return frac_float(std::sin(x * 12.9898f) * 43758.5453f);
	}

	Vec2 random_sample_in_circle() {
		float const t = 2.f * PI * random_float();
		float const u = random_float() + random_float();
		float const r = (u > 1.f) ? 2.f - u : u;
		return { r * std::cos(t), r * std::sin(t) };
	}

	Vec2 vec2_lerp(Vec2 const & v0, Vec2 const & v1, float t) {
		return v0 * (1.f - t) + v1 * t;
	}

	Vec2 vec2_normalize(Vec2 const & v) {
		float length = vec2_length(v);
		return (length > 0.f) ? v / length : math::VEC2_ZERO;
	}

	float vec2_length(Vec2 const & v) {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	float vec2_length_squared(Vec2 const & v) {
		return v.x * v.x + v.y * v.y;
	}
}