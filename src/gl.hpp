
#ifndef NAMESPACE_GL_INCLUDED
#define NAMESPACE_GL_INCLUDED

#define STRINGIFY_GLSL_SHADER(version, shader) "#version " #version "\n" #shader
#include <basic.vert>
#include <basic.frag>
#include <particle.vert>
#include <particle.frag>
#include <star.vert>
#include <star.frag>
#include <screen_quad.vert>
#include <texture.frag>
#include <threshold_texture.frag>
#include <blur_texture.frag>

namespace gl {
	struct VertexBuffer {
		GLuint id;
		uint32_t vert_count;
		uint32_t vert_size;
		uint32_t size_in_bytes;
	};

	struct FrameBuffer {
		GLuint id;
		GLuint texture_id;
		GLuint depth_buffer_id;
		uint32_t width;
		uint32_t height;
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
		VertexBuffer vertex_buffer = {};
		vertex_buffer.vert_count = vert_data_length / vert_size;
		vertex_buffer.vert_size = vert_size;
		vertex_buffer.size_in_bytes = vert_data_length * sizeof(float);

		glGenBuffers(1, &vertex_buffer.id);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.id);
		glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size_in_bytes, vert_data, usage_flag);

		return vertex_buffer;
	}

	FrameBuffer create_frame_buffer(uint32_t width, uint32_t height, GLint min_filter = GL_NEAREST, GLint mag_filter = GL_NEAREST) {
		FrameBuffer frame_buffer = {};
		frame_buffer.width = width;
		frame_buffer.height = height;

		glGenFramebuffers(1, &frame_buffer.id);
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer.id);

		glGenTextures(1, &frame_buffer.texture_id);
		glBindTexture(GL_TEXTURE_2D, frame_buffer.texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer.texture_id, 0);

		glGenRenderbuffers(1, &frame_buffer.depth_buffer_id);
		glBindRenderbuffer(GL_RENDERBUFFER, frame_buffer.depth_buffer_id);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frame_buffer.depth_buffer_id);

		ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return frame_buffer;
	}

	FrameBuffer create_msaa_frame_buffer(uint32_t width, uint32_t height, uint32_t sample_count) {
		FrameBuffer frame_buffer = {};
		frame_buffer.width = width;
		frame_buffer.height = height;
		// frame_buffer.sample_count = sample_count;

		glGenFramebuffers(1, &frame_buffer.id);
	    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer.id);

		glGenTextures(1, &frame_buffer.texture_id);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frame_buffer.texture_id);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample_count, GL_RGB, width, height, false);

		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, frame_buffer.texture_id, 0);

		glGenRenderbuffers(1, &frame_buffer.depth_buffer_id);
		glBindRenderbuffer(GL_RENDERBUFFER, frame_buffer.depth_buffer_id);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, sample_count, GL_DEPTH_COMPONENT16, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frame_buffer.depth_buffer_id);

		ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return frame_buffer;
	}

	void resolve_msaa_frame_buffer(FrameBuffer const & msaa_frame_buffer, FrameBuffer const & resolve_frame_buffer) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msaa_frame_buffer.id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolve_frame_buffer.id);
		glBlitFramebuffer(0, 0, msaa_frame_buffer.width, msaa_frame_buffer.height, 0, 0, resolve_frame_buffer.width, resolve_frame_buffer.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
}

#endif