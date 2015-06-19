
#ifndef NAMESPACE_SYS_INCLUDED
#define NAMESPACE_SYS_INCLUDED

#define NOMINMAX
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define __TOKEN_STRINGIFY(x) #x
#define TOKEN_STRINGIFY(x) __TOKEN_STRINGIFY(x)

#define __ASSERT(x) __assert_func(x, "" #x " : " __FILE__ " : " TOKEN_STRINGIFY(__LINE__))
void __assert_func(bool expression, char const * message) {
	if(!expression) {
		MessageBoxA(0, message, "ASSERT", MB_OK | MB_ICONERROR);
		std::fprintf(stderr, "ASSERT: %s\n", message);
		std::exit(EXIT_FAILURE);
		// *((int *)(0)) = 0;
	}
}

#define ASSERT(x) __ASSERT(x)
#define ARRAY_COUNT(x) (sizeof((x)) / sizeof((x)[0]))

namespace sys {
	float const FLOAT_MAX = 1e+37f;

	char __exe_path_buffer[MAX_PATH];
	char const * __get_exe_path() {
		GetModuleFileNameA(0, __exe_path_buffer, MAX_PATH);
		
		size_t path_length = 0;
		for(size_t i = 0; i < MAX_PATH; i++) {
			if(__exe_path_buffer[i] == '\0') {
				path_length = i;
				break;
			}
		}

		size_t last_slash_index = 0;
		for(size_t i = 0; i < path_length; i++) {
			size_t r_i = (path_length - 1) - i;
			if(__exe_path_buffer[r_i] == '/' || __exe_path_buffer[r_i] == '\\') {
				last_slash_index = r_i;
				break;
			}			
		}

		__exe_path_buffer[last_slash_index + 1] = '\0';
		return __exe_path_buffer;
	}

	char const * exe_path_str = __get_exe_path();
}

#endif