
#ifndef MATH_INCLUDED_HPP
#define MATH_INCLUDED_HPP

//NOTE: Get rid of this?
#include <cmath>

namespace math {
	struct Vec2 {
		float x, y;
	};

	float const PI = 3.14159265359f;
	// float const FLOAT_MAX = 1e+37f;
	float const FLOAT_MAX = 1000.f;

	Vec2 const VEC2_ZERO = { 0.f, 0.f };
	Vec2 const VEC2_ONE = { 1.f, 1.f };

	float min(float x, float y) {
		return (x < y) ? x : y;
	}

	float max(float x, float y) {
		return (x > y) ? x : y;
	}

	float clamp(float x, float u, float v) {
		return max(min(x, v), u);
	}

	float frac(float x) {
		return x - static_cast<int32_t>(x);
	}

	float squared(float x) {
		return x * x;
	}

	float random_float();
	float psuedo_random_float(float x);
	Vec2 random_sample_circle();

	float simplex_noise(float x, float y);

	Vec2 operator+(Vec2 const & x, Vec2 const & y) {
		Vec2 tmp = x;
		tmp.x += y.x;
		tmp.y += y.y;
		return tmp;
	}

	Vec2 operator-(Vec2 const & x, Vec2 const & y) {
		Vec2 tmp = x;
		tmp.x -= y.x;
		tmp.y -= y.y;
		return tmp;
	}

	Vec2 operator*(Vec2 const & x, Vec2 const & y) {
		Vec2 tmp = x;
		tmp.x *= y.x;
		tmp.y *= y.y;
		return tmp;
	}

	Vec2 operator/(Vec2 const & x, Vec2 const & y) {
		Vec2 tmp = x;
		tmp.x /= y.x;
		tmp.y /= y.y;
		return tmp;
	}

	Vec2 operator-(Vec2 const & v) {
		Vec2 tmp = v;
		tmp.x = -v.x;
		tmp.y = -v.y;
		return tmp;
	}

	Vec2 operator+(Vec2 const & v, float x) {
		Vec2 tmp = v;
		tmp.x += x;
		tmp.y += x;
		return tmp;
	}

	Vec2 operator-(Vec2 const & v, float x) {
		Vec2 tmp = v;
		tmp.x -= x;
		tmp.y -= x;
		return tmp;
	}

	Vec2 operator*(Vec2 const & v, float x) {
		Vec2 tmp = v;
		tmp.x *= x;
		tmp.y *= x;
		return tmp;
	}

	Vec2 operator/(Vec2 const & v, float x) {
		Vec2 tmp = v;
		tmp.x /= x;
		tmp.y /= x;
		return tmp;
	}

	float dot(Vec2 const & x, Vec2 const & y) {
		return x.x * y.x + x.y * y.y;
	}

	Vec2 lerp(Vec2 const & x, Vec2 const & y, float t) {
		return x * (1.f - t) + y * t;
	}

	float length(Vec2 const & v) {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	float length_squared(Vec2 const & v) {
		return v.x * v.x + v.y * v.y;
	}

	Vec2 normalize(Vec2 const & v) {
		float const len = length(v);
		return (len > 0.f) ? v / len : math::VEC2_ZERO;
	}
}

#endif