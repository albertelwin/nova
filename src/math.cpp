
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

	float random_float() {
		return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	}

	Vec2 random_sample_in_circle() {
		float const t = 2.f * PI * random_float();
		float const u = random_float() + random_float();
		float const r = (u > 1.f) ? 2.f - u : u;
		return { r * std::cos(t), r * std::sin(t) };
	}

	Vec2 vec2_add(Vec2 const & v0, Vec2 const & v1) {
		return { v0.x + v1.x, v0.y + v1.y };
	}

	Vec2 vec2_sub(Vec2 const & v0, Vec2 const & v1) {
		return { v0.x - v1.x, v0.y - v1.y };
	}

	Vec2 vec2_mul(Vec2 const & v0, Vec2 const & v1) {
		return { v0.x * v0.x, v1.y * v1.y };
	}

	Vec2 vec2_div(Vec2 const & v0, Vec2 const & v1) {
		return { v0.x / v0.x, v1.y / v1.y };
	}

	Vec2 vec2_add_float(Vec2 const & v, float x) {
		return { v.x + x, v.y + x };
	}

	Vec2 vec2_sub_float(Vec2 const & v, float x) {
		return { v.x - x, v.y - x };
	}

	Vec2 vec2_mul_float(Vec2 const & v, float x) {
		return { v.x * x, v.y * x};
	}

	Vec2 vec2_div_float(Vec2 const & v, float x) {
		return { v.x / x, v.y / x};
	}

	Vec2 vec2_normalize(Vec2 const & v) {
		float length = vec2_length(v);
		return (length > 0.f) ? vec2_div_float(v, length) : math::VEC2_ZERO;
	}

	float vec2_length(Vec2 const & v) {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	float vec2_length_squared(Vec2 const & v) {
		return v.x * v.x + v.y * v.y;
	}
}