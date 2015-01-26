
#ifndef MATH_INCLUDED_HPP
#define MATH_INCLUDED_HPP

namespace math {
	struct Vec2 {
		float x, y;
	};

	float const PI = 3.14159265359f;

	Vec2 const VEC2_ZERO = { 0.f, 0.f };
	Vec2 const VEC2_ONE = { 1.f, 1.f };

	float min_float(float x, float y);
	float max_float(float x, float y);
	float clamp_float(float x, float u, float v);
	float frac_float(float x);

	float random_float();
	float psuedo_random_float();
	Vec2 random_sample_circle();

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

	Vec2 vec2_lerp(Vec2 const & v0, Vec2 const & v1, float t);

	Vec2 vec2_normalize(Vec2 const & v);
	float vec2_length(Vec2 const & v);
	float vec2_length_squared(Vec2 const & v);
}

#endif