
#ifndef NAMESPACE_MATH_INCLUDED_HPP
#define NAMESPACE_MATH_INCLUDED_HPP

//NOTE: Get rid of these?
#include <cmath>
#include <cstdlib>

#include <intrin.hpp>

namespace math {
	float const PI = 3.14159265359f;
	float const FLOAT_MAX = 1e+37f;

	float to_radians(float x) {
		return (PI / 180.0f) * x;
	}

	float to_degrees(float x) {
		return (180.0f / PI) * x;
	}

	float min(float x, float y) {
		return (x < y) ? x : y;
	}

	float max(float x, float y) {
		return (x > y) ? x : y;
	}

	float clamp(float x, float u, float v) {
		return max(min(x, v), u);
	}

	float clamp01(float x) {
		return max(min(x, 1.0f), 0.0f);
	}

	float frac(float x) {
		return x - static_cast<int32_t>(x);
	}

	float squared(float x) {
		return x * x;
	}

	float lerp(float x, float y, float t) {
		//TODO: Assert 0 <= t <= 1??
		return x * (1.0f - t) + y * t;
	}

	float rand_float() {
		return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	}

	union Vec2 {
		struct { float x, y; };
		float v[2];
	};

	Vec2 vec2(float x, float y) {
		return { x, y };
	}

	Vec2 vec2(float x) {
		return { x, x };
	}

	Vec2 operator+(Vec2 const & x, Vec2 const & y) {
		Vec2 tmp = x;
		tmp.x += y.x;
		tmp.y += y.y;
		return tmp;
	}

	Vec2 operator+(Vec2 const & v, float x) {
		Vec2 tmp = v;
		tmp.x += x;
		tmp.y += x;
		return tmp;
	}

	Vec2 operator-(Vec2 const & v) {
		Vec2 tmp = v;
		tmp.x = -v.x;
		tmp.y = -v.y;
		return tmp;
	}

	Vec2 operator-(Vec2 const & x, Vec2 const & y) {
		Vec2 tmp = x;
		tmp.x -= y.x;
		tmp.y -= y.y;
		return tmp;
	}

	Vec2 operator-(Vec2 const & v, float x) {
		Vec2 tmp = v;
		tmp.x -= x;
		tmp.y -= x;
		return tmp;
	}

	Vec2 operator*(Vec2 const & x, Vec2 const & y) {
		Vec2 tmp = x;
		tmp.x *= y.x;
		tmp.y *= y.y;
		return tmp;
	}

	Vec2 operator*(Vec2 const & v, float x) {
		Vec2 tmp = v;
		tmp.x *= x;
		tmp.y *= x;
		return tmp;
	}

	Vec2 operator/(Vec2 const & x, Vec2 const & y) {
		Vec2 tmp = x;
		tmp.x /= y.x;
		tmp.y /= y.y;
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
		return x * (1.0f - t) + y * t;
	}

	float length(Vec2 const & v) {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	float length_squared(Vec2 const & v) {
		return v.x * v.x + v.y * v.y;
	}

	Vec2 normalize(Vec2 const & v) {
		float const len = std::sqrt(v.x * v.x + v.y * v.y);
		return (len > 0.0f) ? v / len : vec2(0.0f);
	}

	Vec2 rand_vec2() {
		return { rand_float(), rand_float() };
	}

	union Vec3 {
		struct { float x, y, z; };
		struct { Vec2 xy; float z; };
		struct { float z; Vec2 yz; };
		float v[3];
	};

	Vec3 const VEC3_RIGHT = { 1.0f, 0.0f, 0.0f };
	Vec3 const VEC3_UP = { 0.0f, 1.0f, 0.0f };
	Vec3 const VEC3_FORWARD = { 0.0f, 0.0f, 1.0f };

	Vec3 vec3(float x, float y, float z) {
		return { x, y, z };
	}

	Vec3 vec3(float x) {
		return { x, x, x };
	}

	Vec3 vec3(Vec2 const & xy, float z) {
		return { xy.x, xy.y, z };
	}

	Vec3 operator+(Vec3 const & x, Vec3 const & y) {
		Vec3 tmp = x;
		tmp.x += y.x;
		tmp.y += y.y;
		tmp.z += y.z;
		return tmp;
	}

	Vec3 operator+(Vec3 const & v, float x) {
		Vec3 tmp = v;
		tmp.x += x;
		tmp.y += x;
		tmp.z += x;
		return tmp;
	}

	Vec3 & operator+=(Vec3 & v, float x) {
		v.x += x;
		v.y += x;
		v.z += x;
		return v;
	}

	Vec3 operator-(Vec3 const & x, Vec3 const & y) {
		Vec3 tmp = x;
		tmp.x -= y.x;
		tmp.y -= y.y;
		tmp.z -= y.z;
		return tmp;
	}

	Vec3 operator-(Vec3 const & v) {
		Vec3 tmp;
		tmp.x = -v.x;
		tmp.y = -v.y;
		tmp.z = -v.z;
		return tmp;
	}

	Vec3 operator-(Vec3 const & v, float x) {
		Vec3 tmp = v;
		tmp.x -= x;
		tmp.y -= x;
		tmp.z -= x;
		return tmp;
	}

	Vec3 & operator-=(Vec3 & v, float x) {
		v.x -= x;
		v.y -= x;
		v.z -= x;
		return v;
	}

	Vec3 operator*(Vec3 const & x, Vec3 const & y) {
		Vec3 tmp = x;
		tmp.x *= y.x;
		tmp.y *= y.y;
		tmp.z *= y.z;
		return tmp;
	}

	Vec3 operator*(Vec3 const & v, float x) {
		Vec3 tmp = v;
		tmp.x *= x;
		tmp.y *= x;
		tmp.z *= x;
		return tmp;
	}

	Vec3 & operator*=(Vec3 & v, float x) {
		v.x *= x;
		v.y *= x;
		v.z *= x;
		return v;
	}

	Vec3 operator/(Vec3 const & x, Vec3 const & y) {
		Vec3 tmp = x;
		tmp.x /= y.x;
		tmp.y /= y.y;
		tmp.z /= y.z;
		return tmp;
	}

	Vec3 operator/(Vec3 const & v, float x) {
		Vec3 tmp = v;
		tmp.x /= x;
		tmp.y /= x;
		tmp.z /= x;
		return tmp;
	}

	Vec3 & operator/=(Vec3 & v, float x) {
		v.x /= x;
		v.y /= x;
		v.z /= x;
		return v;
	}

	Vec3 normalize(Vec3 const & v) {
		float const len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		return (len > 0.0f) ? v / len : vec3(0.0f);
	}

	float dot(Vec3 const & x, Vec3 const & y) {
		return x.x * y.x + x.y * y.y + x.z * y.z;
	}

	Vec3 cross(Vec3 const & x, Vec3 const & y) {
		return { x.y * y.z - x.z * y.y, x.z * y.x - x.x * y.z, x.x * y.y - x.y * y.x };
	}

	Vec3 rand_vec3() {
		return { rand_float(), rand_float(), rand_float() };
	}

	union Vec4 {
		struct { float x, y, z, w; };
		struct { Vec3 xyz; float w; };
		struct { Vec2 xy, zw; };
		float v[4];
	};

	Vec4 vec4(float x, float y, float z, float w) {
		return { x, y, z, w };
	}

	Vec4 vec4(float x) {
		return { x, x, x, x };
	}

	Vec4 vec4(Vec3 const & v, float w) {
		return { v.x, v.y, v.z, w };
	}

	Vec4 vec4(Vec2 const & v, float z, float w) {
		return { v.x, v.y, z, w };
	}

	struct Mat4 {
		float v[16];
	};

	Mat4 const MAT4_IDENTITY = {
		 1.0f, 0.0f, 0.0f, 0.0f,
		 0.0f, 1.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f, 0.0f, 0.0f, 1.0f,
	};

	Mat4 operator*(Mat4 const & x, Mat4 const & y) {
		Mat4 tmp;

		tmp.v[ 0] = y.v[ 0] * x.v[ 0] + y.v[ 1] * x.v[ 4] + y.v[ 2] * x.v[ 8] + y.v[ 3] * x.v[12];
		tmp.v[ 1] = y.v[ 0] * x.v[ 1] + y.v[ 1] * x.v[ 5] + y.v[ 2] * x.v[ 9] + y.v[ 3] * x.v[13];
		tmp.v[ 2] = y.v[ 0] * x.v[ 2] + y.v[ 1] * x.v[ 6] + y.v[ 2] * x.v[10] + y.v[ 3] * x.v[14];
		tmp.v[ 3] = y.v[ 0] * x.v[ 3] + y.v[ 1] * x.v[ 7] + y.v[ 2] * x.v[11] + y.v[ 3] * x.v[15];

		tmp.v[ 4] = y.v[ 4] * x.v[ 0] + y.v[ 5] * x.v[ 4] + y.v[ 6] * x.v[ 8] + y.v[ 7] * x.v[12];
		tmp.v[ 5] = y.v[ 4] * x.v[ 1] + y.v[ 5] * x.v[ 5] + y.v[ 6] * x.v[ 9] + y.v[ 7] * x.v[13];
		tmp.v[ 6] = y.v[ 4] * x.v[ 2] + y.v[ 5] * x.v[ 6] + y.v[ 6] * x.v[10] + y.v[ 7] * x.v[14];
		tmp.v[ 7] = y.v[ 4] * x.v[ 3] + y.v[ 5] * x.v[ 7] + y.v[ 6] * x.v[11] + y.v[ 7] * x.v[15];

		tmp.v[ 8] = y.v[ 8] * x.v[ 0] + y.v[ 9] * x.v[ 4] + y.v[10] * x.v[ 8] + y.v[11] * x.v[12];
		tmp.v[ 9] = y.v[ 8] * x.v[ 1] + y.v[ 9] * x.v[ 5] + y.v[10] * x.v[ 9] + y.v[11] * x.v[13];
		tmp.v[10] = y.v[ 8] * x.v[ 2] + y.v[ 9] * x.v[ 6] + y.v[10] * x.v[10] + y.v[11] * x.v[14];
		tmp.v[11] = y.v[ 8] * x.v[ 3] + y.v[ 9] * x.v[ 7] + y.v[10] * x.v[11] + y.v[11] * x.v[15];

		tmp.v[12] = y.v[12] * x.v[ 0] + y.v[13] * x.v[ 4] + y.v[14] * x.v[ 8] + y.v[15] * x.v[12];
		tmp.v[13] = y.v[12] * x.v[ 1] + y.v[13] * x.v[ 5] + y.v[14] * x.v[ 9] + y.v[15] * x.v[13];
		tmp.v[14] = y.v[12] * x.v[ 2] + y.v[13] * x.v[ 6] + y.v[14] * x.v[10] + y.v[15] * x.v[14];
		tmp.v[15] = y.v[12] * x.v[ 3] + y.v[13] * x.v[ 7] + y.v[14] * x.v[11] + y.v[15] * x.v[15];

		return tmp;
	}

	//TODO: Does this actually work??
	Vec4 operator*(Mat4 const & m, Vec4 const & v) {
		Vec4 tmp;
		tmp.x = v.x * m.v[ 0] + v.y * m.v[ 1] + v.z * m.v[ 2] + v.w * m.v[ 3];
		tmp.y = v.x * m.v[ 4] + v.y * m.v[ 5] + v.z * m.v[ 6] + v.w * m.v[ 7];
		tmp.z = v.x * m.v[ 8] + v.y * m.v[ 9] + v.z * m.v[10] + v.w * m.v[11];
		tmp.w = v.x * m.v[12] + v.y * m.v[13] + v.z * m.v[14] + v.w * m.v[15];
		return tmp;
	}

	Mat4 inverse(Mat4 const & x) {
		// Mat4 adj;

		// float deter;
		// ASSERT(deter > 0.0f);
		// return adj / deter;

		return MAT4_IDENTITY;
	}

	Mat4 translate(float x, float y, float z) {
		return {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			   x,    y,    z, 1.0f,
		};		
	}

	Mat4 translate(Vec3 const & v) {
		return {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			 v.x,  v.y,  v.z, 1.0f,
		};
	}

	Mat4 rotate_around_axis(Vec3 const & a, float t) {
		float sin_t = intrin::sin(t);
		float cos_t = intrin::cos(t);
		float i_cos_t = (1.0f - cos_t);

		Vec3 v = normalize(a);
		float vx2 = v.x * v.x;
		float vy2 = v.y * v.y;
		float vz2 = v.z * v.z;
		float vxy = v.x * v.y;
		float vxz = v.x * v.z;
		float vyz = v.y * v.z;

		return {
			cos_t + vx2 * i_cos_t,       vxy * i_cos_t + v.z * sin_t, vxz * i_cos_t - v.y * sin_t, 0.0f,
			vxy * i_cos_t - v.z * sin_t, cos_t + vy2 * i_cos_t,       vyz * i_cos_t + v.x * sin_t, 0.0f,
			vxz * i_cos_t + v.y * sin_t, vyz * i_cos_t - v.x * sin_t, cos_t + vz2 * i_cos_t,       0.0f,
			0.0f,                        0.0f,                        0.0f,                        1.0f,
		};

	}

	Mat4 rotate_around_x(float t) {
		float sin_t = intrin::sin(t);
		float cos_t = intrin::cos(t);

		return {
			1.0f,  0.0f,  0.0f, 0.0f,
			0.0f, cos_t, sin_t, 0.0f,
			0.0f,-sin_t, cos_t, 0.0f,
			0.0f,  0.0f,  0.0f, 1.0f,
		};		
	}

	Mat4 rotate_around_y(float t) {
		float sin_t = intrin::sin(t);
		float cos_t = intrin::cos(t);

		return {
			cos_t, 0.0f,-sin_t, 0.0f,
			 0.0f, 1.0f,  0.0f, 0.0f,
			sin_t, 0.0f, cos_t, 0.0f,
			 0.0f, 0.0f,  0.0f, 1.0f,
		};
	}

	Mat4 rotate_around_z(float t) {
		float sin_t = intrin::sin(t);
		float cos_t = intrin::cos(t);

		return {
			 cos_t, sin_t, 0.0f, 0.0f,
			-sin_t, cos_t, 0.0f, 0.0f,
			  0.0f,  0.0f, 1.0f, 0.0f,
			  0.0f,  0.0f, 0.0f, 1.0f,
		};
	}

	Mat4 scale(float x) {
		return {
			  x, 0.0f, 0.0f, 0.0f,
			0.0f,   x, 0.0f, 0.0f,
			0.0f, 0.0f,   x, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};		
	}

	Mat4 scale(float x, float y, float z) {
		return {
			  x, 0.0f, 0.0f, 0.0f,
			0.0f,   y, 0.0f, 0.0f,
			0.0f, 0.0f,   z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};
	}

	Mat4 scale(Vec3 const & v) {
		return {
			 v.x, 0.0f, 0.0f, 0.0f,
			0.0f,  v.y, 0.0f, 0.0f,
			0.0f, 0.0f,  v.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};
	}

	Mat4 look_at(Vec3 const & pos, Vec3 const & at, Vec3 const & up) {
		Vec3 z = normalize(at - pos);
		Vec3 x = normalize(cross(z, normalize(up)));
		Vec3 y = cross(x, z);
		return {
			          x.x,         y.x,        -z.x, 0.0f,
			          x.y,         y.y,        -z.y, 0.0f,
			          x.z,         y.z,        -z.z, 0.0f,
			 -dot(x, pos),-dot(y, pos), dot(z, pos), 1.0f,
		};
	}

	Mat4 perspective_projection(float aspect_ratio, float field_of_view, float near_plane, float far_plane) {
		float tan_fov_over_2 = intrin::tan(math::to_radians(field_of_view) / 2.0f);
		return {
			1.0f / (aspect_ratio * tan_fov_over_2), 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f / tan_fov_over_2, 0.0f, 0.0f,
			0.0f, 0.0f, -(far_plane + near_plane) / (far_plane - near_plane), -1.0f,
			0.0f, 0.0f, -(2.0f * far_plane * near_plane) / (far_plane - near_plane), 0.0f,						
		};
	}

	struct Ray {
		Vec3 o;
		Vec3 d;
	};

	Ray ray(Vec3 o, Vec3 d) {
		return { o, d };
	}

	float ray_plane_intersection(Ray const & r, Vec3 const & p_n, float p_d) {
		float t = -1.0f;

		float denom = dot(p_n, r.d);
		if(intrin::abs(denom) > 0.0f) {
			t = -(dot(p_n, r.o) + p_d) / denom;
		}

		return t;
	}
}

#endif