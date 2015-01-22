
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

	float random_float();
	Vec2 random_sample_circle();
	
	Vec2 vec2_add(Vec2 const & v0, Vec2 const & v1);
	Vec2 vec2_sub(Vec2 const & v0, Vec2 const & v1);
	Vec2 vec2_mul(Vec2 const & v0, Vec2 const & v1);
	Vec2 vec2_div(Vec2 const & v0, Vec2 const & v1);

	Vec2 vec2_add_float(Vec2 const & v, float x);
	Vec2 vec2_sub_float(Vec2 const & v, float x);
	Vec2 vec2_mul_float(Vec2 const & v, float x);
	Vec2 vec2_div_float(Vec2 const & v, float x);

	Vec2 vec2_lerp(Vec2 const & v0, Vec2 const & v1, float t);
	Vec2 vec2_clamp(Vec2 const &v, float x, float y);

	Vec2 vec2_normalize(Vec2 const & v);
	float vec2_length(Vec2 const & v);
	float vec2_length_squared(Vec2 const & v);
}

#endif