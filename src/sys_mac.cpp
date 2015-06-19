#include <sys.hpp>
#include <libproc.h>
#include <unistd.h>
#include <iostream>


namespace sys {

	char const * __get_exe_path() {
		static char __exe_path_buffer[PROC_PIDPATHINFO_MAXSIZE];
		proc_pidpath(getpid(), __exe_path_buffer, sizeof(__exe_path_buffer));

		size_t path_length = 0;
		for(size_t i = 0; i < PROC_PIDPATHINFO_MAXSIZE; i++) {
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