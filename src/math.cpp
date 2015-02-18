
#include <math.hpp>

#include <cstdlib>

namespace math {
	float random_float() {
		return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	}

	float pseudo_random_float(float x) {
		return frac(std::sin(x * 12.9898f) * 43758.5453f);
		// return simplex_noise(x * 512.f, 0.f);
	}

	Vec2 random_sample_in_circle() {
		float const t = 2.f * PI * random_float();
		float const u = random_float() + random_float();
		float const r = (u > 1.f) ? 2.f - u : u;
		return { r * std::cos(t), r * std::sin(t) };
	}

	uint8_t const noise_permutations[] = { 
		151, 160, 137,  91,  90,  15, 131,  13,
		201,  95,  96,  53, 194, 233,   7, 225,
		140,  36, 103,  30,  69, 142,   8,  99,
		 37, 240,  21,  10,  23, 190,   6, 148,
		247, 120, 234,  75,   0,  26, 197,  62,
		 94, 252, 219, 203, 117,  35,  11,  32,
		 57, 177,  33,  88, 237, 149,  56,  87,
		174,  20, 125, 136, 171, 168,  68, 175,
		 74, 165,  71, 134, 139,  48,  27, 166,
		 77, 146, 158, 231,  83, 111, 229, 122,
		 60, 211, 133, 230, 220, 105,  92,  41,
		 55,  46, 245,  40, 244, 102, 143,  54,
		 65,  25,  63, 161,   1, 216,  80,  73,
		209,  76, 132, 187, 208,  89,  18, 169,
		200, 196, 135, 130, 116, 188, 159,  86,
		164, 100, 109, 198, 173, 186,   3,  64,
		 52, 217, 226, 250, 124, 123,   5, 202,
		 38, 147, 118, 126, 255,  82,  85, 212,
		207, 206,  59, 227,  47,  16,  58,  17,
		182, 189,  28,  42, 223, 183, 170, 213,
		119, 248, 152,   2,  44, 154, 163,  70,
		221, 153, 101, 155, 167,  43, 172,   9,
		129,  22,  39, 253,  19,  98, 108, 110,
		 79, 113, 224, 232, 178, 185, 112, 104,
		218, 246,  97, 228, 251,  34, 242, 193,
		238, 210, 144,  12, 191, 179, 162, 241,
		 81,  51, 145, 235, 249,  14, 239, 107,
		 49, 192, 214,  31, 181, 199, 106, 157,
		184,  84, 204, 176, 115, 121,  50,  45,
		127,   4, 150, 254, 138, 236, 205,  93,
		222, 114,  67,  29,  24,  72, 243, 141,
		128, 195,  78,  66, 215,  61, 156, 180,

		151, 160, 137,  91,  90,  15, 131,  13,
		201,  95,  96,  53, 194, 233,   7, 225,
		140,  36, 103,  30,  69, 142,   8,  99,
		 37, 240,  21,  10,  23, 190,   6, 148,
		247, 120, 234,  75,   0,  26, 197,  62,
		 94, 252, 219, 203, 117,  35,  11,  32,
		 57, 177,  33,  88, 237, 149,  56,  87,
		174,  20, 125, 136, 171, 168,  68, 175,
		 74, 165,  71, 134, 139,  48,  27, 166,
		 77, 146, 158, 231,  83, 111, 229, 122,
		 60, 211, 133, 230, 220, 105,  92,  41,
		 55,  46, 245,  40, 244, 102, 143,  54,
		 65,  25,  63, 161,   1, 216,  80,  73,
		209,  76, 132, 187, 208,  89,  18, 169,
		200, 196, 135, 130, 116, 188, 159,  86,
		164, 100, 109, 198, 173, 186,   3,  64,
		 52, 217, 226, 250, 124, 123,   5, 202,
		 38, 147, 118, 126, 255,  82,  85, 212,
		207, 206,  59, 227,  47,  16,  58,  17,
		182, 189,  28,  42, 223, 183, 170, 213,
		119, 248, 152,   2,  44, 154, 163,  70,
		221, 153, 101, 155, 167,  43, 172,   9,
		129,  22,  39, 253,  19,  98, 108, 110,
		 79, 113, 224, 232, 178, 185, 112, 104,
		218, 246,  97, 228, 251,  34, 242, 193,
		238, 210, 144,  12, 191, 179, 162, 241,
		 81,  51, 145, 235, 249,  14, 239, 107,
		 49, 192, 214,  31, 181, 199, 106, 157,
		184,  84, 204, 176, 115, 121,  50,  45,
		127,   4, 150, 254, 138, 236, 205,  93,
		222, 114,  67,  29,  24,  72, 243, 141,
		128, 195,  78,  66, 215,  61, 156, 180,
	};

	int8_t const noise_gradients[12][2] = {
		{ 1, 1 }, {-1, 1 }, { 1,-1 }, {-1,-1 },
		{ 1, 0 }, {-1, 0 }, { 1, 0 }, {-1, 0 },
		{ 0, 1 }, { 0,-1 }, { 0, 1 }, { 0,-1 },
	};

	int32_t floor_float_to_int(float x) {
		int32_t x_int = static_cast<int32_t>(x);
		return (x > 0) ? x_int : x_int - 1;
	}

	float simplex_noise(float x, float y) {
		float const f2 = 0.5f * (std::sqrtf(3.0f) - 1.0f);
		float const g2 = (3.0f - std::sqrtf(3.0f)) / 6.0f;

	    float const s = (x + y) * f2;
	    int32_t const i = floor_float_to_int(x + s);
	    int32_t const j = floor_float_to_int(y + s);

	    float const t = (i + j) * g2;

	    float const x0 = x - (i - t);
	    float const y0 = y - (j - t);

	    int32_t const i1 = (x0 > y0) ? 1 : 0;
	    int32_t const j1 = (x0 > y0) ? 0 : 1;

	    float const x1 = x0 - i1 + g2;
	    float const y1 = y0 - j1 + g2;
	    float const x2 = x0 - 1.0f + 2.0f * g2;
	    float const y2 = y0 - 1.0f + 2.0f * g2;

	    int32_t const ii = i & 0xFF;
	    int32_t const jj = j & 0xFF;

	    int32_t const g_idx0 = noise_permutations[ii + noise_permutations[jj]] % 12;
	    int32_t const g_idx1 = noise_permutations[ii + i1 + noise_permutations[jj + j1]] % 12;
	    int32_t const g_idx2 = noise_permutations[ii + 1 + noise_permutations[jj + 1]] % 12;

	    float const t0 = 0.5f - (x0 * x0) - (y0 * y0);
	    float const t1 = 0.5f - (x1 * x1) - (y1 * y1);
	    float const t2 = 0.5f - (x2 * x2) - (y2 * y2);

	    float const n0 = (t0 < 0.f) ? 0.f : (t0 * t0 * t0 * t0) * (noise_gradients[g_idx0][0] * x0 + noise_gradients[g_idx0][1] * y0);
	    float const n1 = (t1 < 0.f) ? 0.f : (t1 * t1 * t1 * t1) * (noise_gradients[g_idx1][0] * x1 + noise_gradients[g_idx1][1] * y1);
	    float const n2 = (t2 < 0.f) ? 0.f : (t2 * t2 * t2 * t2) * (noise_gradients[g_idx2][0] * x2 + noise_gradients[g_idx2][1] * y2);

	   	float const raw_noise = n0 + n1 + n2;
	    return ((70.0f * raw_noise) + 1.f) * 0.5f;
	}
}