
#ifndef NAMESPACE_GL_INCLUDED
#define NAMESPACE_GL_INCLUDED

#define STRINGIFY_GLSL_SHADER(version, shader) "#version " #version "\n" #shader
#include <basic.vert>
#include <basic.frag>
#include <particle.vert>
#include <particle.frag>
#include <star.vert>
#include <star.frag>

namespace gl {
	struct VertexBuffer {
		uint32_t vert_count;
		uint32_t vert_size;
		uint32_t size_in_bytes;
		GLuint id;
	};

	GLuint compile_shader_from_source(char const * shader_src, GLenum shader_type) {
		GLuint shader_id = glCreateShader(shader_type);
		glShaderSource(shader_id, 1, &shader_src, 0);
		glCompileShader(shader_id);

		GLint compile_result;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_result);
		if(compile_result == GL_FALSE) {
			GLint info_log_length;
			glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
			char * info_log = new char[info_log_length];
			glGetShaderInfoLog(shader_id, info_log_length, 0, info_log);

			std::printf("%s: %s\n", shader_src, info_log);
			delete[] info_log;
		}

		return shader_id;
	}

	GLuint link_shader_program(GLuint vert_id, GLuint frag_id) {
		GLuint program_id = glCreateProgram();
		glAttachShader(program_id, vert_id);
		glAttachShader(program_id, frag_id);
		glLinkProgram(program_id);

		GLint link_result;
		glGetProgramiv(program_id, GL_LINK_STATUS, &link_result);
		if(link_result == GL_FALSE) {
			GLint info_log_length;
			glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
			char * info_log = new char[info_log_length];
			glGetProgramInfoLog(program_id, info_log_length, 0, info_log);

			std::printf("%s\n", info_log);
			delete[] info_log;
		}

		return program_id;
	}

	VertexBuffer create_vertex_buffer(float const * vert_data, uint32_t vert_data_length, uint32_t vert_size, GLenum usage_flag) {
		uint32_t total_size_in_bytes = vert_data_length * sizeof(float);

		GLuint vertex_buffer_id;
		glGenBuffers(1, &vertex_buffer_id);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
		glBufferData(GL_ARRAY_BUFFER, total_size_in_bytes, vert_data, usage_flag);

		return { vert_data_length / vert_size, vert_size, total_size_in_bytes, vertex_buffer_id };
	}
}

#endif