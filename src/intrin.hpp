
#ifndef NAMESPACE_INTRIN_INCLUDED
#define NAMESPACE_INTRIN_INCLUDED

#include <math.h>

namespace intrin {
	float sin(float x) {
		return sinf(x);
	}

	float cos(float x) {
		return cosf(x);
	}

	float tan(float x) {
		return tanf(x);
	}

	float abs(float x) {
		return fabsf(x);
	}
}

#endif