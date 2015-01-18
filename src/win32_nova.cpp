
#include <windows.h>

#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/glfw3.h>

#include <cstdio>
#include <cstdint>
#include <ctime>
#include <string>
#include <vector>

#include <math.hpp>

#include <math.cpp>

//NOTE: Shaders go here!
#define STRINGIFY_GLSL_SHADER(version, shader) "#version " #version "\n" #shader
#include <basic.vert>
#include <basic.frag>

struct Particle {
	math::Vec2 position;
	math::Vec2 velocity;
	float mass;
};

float get_particle_radius(float mass) {
	return std::sqrt(mass / math::PI) * 0.0032;
}

void error_callback(int e, char const * desc) {
	std::printf("ERROR: %d, %s\n", e, desc);
}

void key_callback(GLFWwindow * window, int key, int scan_code, int action, int mods) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

float get_current_time() {
	return static_cast<float>(glfwGetTime());
}

std::string get_executable_directory() {
	char str_buffer[MAX_PATH];
	GetModuleFileName(GetModuleHandle(0), str_buffer, MAX_PATH);

	std::string const full_path = str_buffer;
	size_t const index = full_path.find_last_of("/\\");
	return full_path.substr(0, index + 1);
}

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

int main() {
	glfwSetErrorCallback(error_callback);

	if(!glfwInit()) {
		return 0;
	}

	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//GLFWwindow * window = glfwCreateWindow(1366, 768, "Nova", glfwGetPrimaryMonitor(), 0);
	GLFWwindow * window = glfwCreateWindow(512, 512, "Nova", 0, 0);
	if(!window) {
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		return 0;
	}

	GLuint vertex_array_id;
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	GLuint const vert_id = compile_shader_from_source(BASIC_VERT_SRC, GL_VERTEX_SHADER);
	GLuint const frag_id = compile_shader_from_source(BASIC_FRAG_SRC, GL_FRAGMENT_SHADER);

	GLuint const program_id = link_shader_program(vert_id, frag_id);
	glUseProgram(program_id);

	GLuint const position_id = glGetUniformLocation(program_id, "position");
	GLuint const radius_id = glGetUniformLocation(program_id, "radius");
	GLuint const aspect_id = glGetUniformLocation(program_id, "aspect");

	uint32_t const division_count = 64;
	float const division_step = (math::PI * 2.f) / static_cast<float>(division_count);

	uint32_t const mesh_vert_count = division_count * 3;
	std::vector<GLfloat> mesh_verts;
	mesh_verts.reserve(mesh_vert_count * 2);

	for(uint32_t i = 0; i < division_count; i++) {
		float const u = division_step * static_cast<float>(i + 0);
		float const v = division_step * static_cast<float>(i + 1);

		mesh_verts.push_back(std::cos(u));
		mesh_verts.push_back(std::sin(u));

		mesh_verts.push_back(std::cos(v));
		mesh_verts.push_back(std::sin(v));

		mesh_verts.push_back(0.f);
		mesh_verts.push_back(0.f);
	}

	GLuint vertex_buffer_id;
	glGenBuffers(1, &vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, mesh_vert_count * sizeof(float) * 2, &mesh_verts[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, static_cast<void *>(0));

	glClearColor(0.f, 0.f, 0.f, 0.f);

	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

	uint32_t const max_particle_count = 15000;
	std::vector<Particle> particles;
	particles.reserve(max_particle_count);

	particles.push_back({ math::VEC2_ZERO, math::VEC2_ZERO, 2000.f });
	while(particles.size() < max_particle_count) {
		particles.push_back({  math::random_sample_in_circle(), math::VEC2_ZERO, 1.f + math::random_float() * 1.2f });
	}

	float this_frame_time = get_current_time();

	while(!glfwWindowShouldClose(window)) {
		float const last_frame_time = this_frame_time;
		this_frame_time = get_current_time();

		float const delta_time = this_frame_time - last_frame_time;

		glfwPollEvents();

		int screen_dimension_x_int;
		int screen_dimension_y_int;
		glfwGetWindowSize(window, &screen_dimension_x_int, &screen_dimension_y_int);

		float const screen_dimension_x = static_cast<float>(screen_dimension_x_int);
		float const screen_dimension_y = static_cast<float>(screen_dimension_y_int);

		double mouse_x;
		double mouse_y;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);

		// math::Vec2 const screen_coords = {
		// 	(static_cast<float>(mouse_x) / (screen_dimension_x * 0.5f) - 1.f) * (screen_dimension_x / screen_dimension_y),
		// 	(static_cast<float>(mouse_y) / (screen_dimension_y * 0.5f) - 1.f) * -1.f,
		// };

		math::Vec2 const screen_coords = {
			std::cos(this_frame_time) * 0.6f,
			std::sin(this_frame_time * 2.f) * 0.6f,
		};

		particles[0].position = screen_coords;

		// //if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		// if(particles.size() < max_particle_count) {
		// 	for(uint32_t i = 0; i < 10; i++) {
		// 		//math::Vec2 const random_offset = math::vec2_mul_float(math::random_sample_in_circle(), 0.2f);
		// 		//math::Vec2 const position = math::vec2_add(screen_coords, random_offset);
		// 		//float const mass = 1.f + math::random_float(); 
		// 		math::Vec2 const position = math::random_sample_in_circle();
		// 		float const mass = 1.f + math::random_float();

		// 		particles.push_back({ position, math::VEC2_ZERO, mass });
		// 	}
		// }

		auto min_size_t = [](size_t x, size_t y) -> size_t {
			return x < y ? x : y;
		};

		for(uint32_t i = 0; i < particles.size(); i++) {
			math::Vec2 gravity = math::VEC2_ZERO;

			for(uint32_t j = 0; j < ((i == 0) ? particles.size() : min_size_t(2, particles.size())); j++) {
				if(j != i && i < particles.size()) {
					math::Vec2 const position = math::vec2_sub(particles[j].position, particles[i].position);
					float const length_squared = math::vec2_length_squared(position);

					float const min_distance = get_particle_radius(particles[i].mass) + get_particle_radius(particles[j].mass);
					if(length_squared < (min_distance * min_distance)) {
						if(particles[i].mass >= particles[j].mass) {
							particles[i].mass += particles[j].mass;

							particles.erase(particles.begin() + j);
							i -= (j < i) ? 1 : 0;
							j -= 1; 
						}
					}
					else {
						float const gravity_constant = 0.00002;
						float const particle_gravity = gravity_constant * (particles[i].mass * particles[j].mass) / length_squared;
						gravity = math::vec2_add(gravity, math::vec2_mul_float(math::vec2_normalize(position), particle_gravity)); 
					}
				}
			}

			if(i < particles.size()) {
				float const gravity_length = math::vec2_length(gravity);
				gravity = math::vec2_mul_float(math::vec2_normalize(gravity), math::min_float(gravity_length, 1.f));
				//math::Vec2 const acceleration = math::vec2_div_float(gravity, particles[i].mass);

				math::Vec2 const acceleration = gravity;

				particles[i].velocity = math::vec2_add(particles[i].velocity, math::vec2_mul_float(acceleration, delta_time));
				particles[i].position = math::vec2_add(particles[i].position, math::vec2_mul_float(math::vec2_div_float(particles[i].velocity, particles[i].mass), delta_time));
			}
		}

		char txt_buffer[256] = {};
		std::sprintf(txt_buffer, "nova, dt: %.3fs", delta_time);
		glfwSetWindowTitle(window, txt_buffer);

		glViewport(0, 0, screen_dimension_x, screen_dimension_y);
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform1f(aspect_id, screen_dimension_y / screen_dimension_x);

		for(auto const & it : particles) {
			glUniform2f(position_id, it.position.x, it.position.y);
			glUniform1f(radius_id, get_particle_radius(it.mass));
			glDrawArrays(GL_TRIANGLES, 0, mesh_vert_count);
		}

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}