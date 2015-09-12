
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <sys.hpp>

namespace {
	char __exe_path_buffer[MAX_PATH];
}

namespace sys {
	char const * get_exe_path() {
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
}