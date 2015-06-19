
#ifndef NAMESPACE_SYS_INCLUDED
#define NAMESPACE_SYS_INCLUDED

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define __TOKEN_STRINGIFY(x) #x
#define TOKEN_STRINGIFY(x) __TOKEN_STRINGIFY(x)

#define __ASSERT(x) __assert_func(x, "" #x " : " __FILE__ " : " TOKEN_STRINGIFY(__LINE__))
void __assert_func(bool expression, char const * message) {
	if(!expression) {
		//MessageBoxA(0, message, "ASSERT", MB_OK | MB_ICONERROR);
		//*(static_cast<int *>(0)) = 0;
	}
}

#define ASSERT(x) __ASSERT(x)
#define ARRAY_COUNT(x) (sizeof((x)) / sizeof((x)[0]))

namespace sys {

	constexpr float FLOAT_MAX = 1e+37f;

	char const * __get_exe_path();
}

#endif