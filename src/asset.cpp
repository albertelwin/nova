
#include <asset.hpp>

#include <cstdio>

namespace asset {
	Model load_obj_from_file(char const * file_path) {
		std::FILE * file_ptr = std::fopen(file_path, "r");
		ASSERT(file_ptr != 0);

		std::fseek(file_ptr, 0, SEEK_END);
		long file_size = std::ftell(file_ptr);
		std::rewind(file_ptr);

		char * file_buffer = new char[file_size];
		size_t read_result = std::fread(file_buffer, 1, file_size, file_ptr);
		ASSERT(read_result == file_size);

		uint32_t vertex_count = 0;
		uint32_t index_count = 0;
		for(size_t i = 0; i < file_size; i++) {
			if(file_buffer[i] == '\n') {
				char line_prefix = file_buffer[++i];
				switch(line_prefix) {
					case 'v': {
						vertex_count++;
						break;
					}

					case 'f': {
						index_count++;
						break;
					}
				}
			}
		}

		float * vertex_array = new float[vertex_count * 3];
		uint32_t vertex_array_size = 0;

		int32_t * index_array = new int32_t[index_count * 3];
		uint32_t index_array_size = 0;

		for(int32_t i = 0; i < file_size; i++) {
			if(file_buffer[i] == '\n') {
				char line_prefix = file_buffer[++i];
				switch(line_prefix) {
					case 'v': {
						i++;

						char float_buffer[256];
						uint32_t float_buffer_size = 0;
						for(uint32_t x = 0; x < 3; x++) {
							i++;
							while(file_buffer[i] != ' ' && file_buffer[i] != '\n') {
								float_buffer[float_buffer_size++] = file_buffer[i++];
							}
							float_buffer[float_buffer_size++] = '\0';
							vertex_array[vertex_array_size++] = (float)std::atof(float_buffer);
							float_buffer_size = 0;
						}

						i--;
						break;						
					}

					case 'f': {
						i++;

						char int_buffer[128];
						uint32_t int_buffer_size = 0;
						for(uint32_t x = 0; x < 3; x++) {
							i++;
							while(file_buffer[i] != ' ' && file_buffer[i] != '\n') {
								int_buffer[int_buffer_size++] = file_buffer[i++];
							}
							int_buffer[int_buffer_size++] = '\0';
							index_array[index_array_size++] = (int32_t)std::atoi(int_buffer) - 1;
							int_buffer_size = 0;
						}

						i--;
						break;
					}
				}
			}
		}

		Model model = {};
		model.vert_data_length = index_array_size * 3;
		model.vert_data = new float[model.vert_data_length];
		for(uint32_t i = 0; i < index_array_size; i++) {
			uint32_t v_idx = index_array[i] * 3;
			ASSERT(v_idx + 2 <= vertex_array_size);

			model.vert_data[i * 3 + 0] = vertex_array[v_idx + 0];
			model.vert_data[i * 3 + 1] = vertex_array[v_idx + 1];
			model.vert_data[i * 3 + 2] = vertex_array[v_idx + 2];
		}

		delete[] vertex_array;
		delete[] index_array;

		delete[] file_buffer;
		std::fclose(file_ptr);

		return model;
	}
}