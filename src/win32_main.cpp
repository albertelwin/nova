
#include <windows.h>
#include <xinput.h>

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

#define STRINGIFY_GLSL_SHADER(version, shader) "#version " #version "\n" #shader
#include <basic.vert>
#include <basic.frag>
#include <particle.vert>
#include <particle.frag>

typedef DWORD WINAPI XInputGetStateProc(DWORD dwUserIndex, XINPUT_STATE *pState);
DWORD WINAPI x_input_get_state_stub(DWORD dwUserIndex, XINPUT_STATE *pState) {
	return ERROR_DEVICE_NOT_CONNECTED;
}

XInputGetStateProc * load_x_input_library() {
	HMODULE x_input_library = LoadLibraryA("xinput1_3.dll");
	if(x_input_library) {
		XInputGetStateProc * x_input_get_state_proc = (XInputGetStateProc *)(GetProcAddress(x_input_library, "XInputGetState"));
		if(x_input_get_state_proc) {
			return x_input_get_state_proc;
		}
	}

	return x_input_get_state_stub;
}


XInputGetStateProc * x_input_get_state = load_x_input_library();
#define XInputGetState x_input_get_state

struct Particle {
	math::Vec2 position;
	math::Vec2 velocity;
	float mass;
};

struct VertexBuffer {
	uint32_t vert_count;
	uint32_t vert_size;
	uint32_t size_in_bytes;
	GLuint id;
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

GLuint gl_compile_shader_from_source(char const * shader_src, GLenum shader_type) {
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

GLuint gl_link_shader_program(GLuint vert_id, GLuint frag_id) {
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

VertexBuffer gl_create_vertex_buffer(uint32_t vert_count, uint32_t vert_size, GLfloat const * vert_memory, GLenum usage_flag) {
	uint32_t const total_size_in_bytes = vert_count * vert_size * sizeof(float);

	GLuint vertex_buffer_id;
	glGenBuffers(1, &vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, total_size_in_bytes, vert_memory, usage_flag);

	return { vert_count, vert_size, total_size_in_bytes, vertex_buffer_id };
}

math::Vec2 apply_dead_zone_mapping(math::Vec2 const & stick) {
	float const dead_zone = 0.12f;
	float const valid_range = 1.f - dead_zone;
	return (math::vec2_length(stick) > dead_zone) ? math::vec2_div_float(math::vec2_sub_float(stick, dead_zone), valid_range) : math::VEC2_ZERO;
};

int main() {
	glfwSetErrorCallback(error_callback);

	if(!glfwInit()) {
		return 0;
	}

	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	bool const is_full_screen = false;

	uint32_t const initial_window_size_x = is_full_screen ? 1366 : 960;
	uint32_t const initial_window_size_y = is_full_screen ? 768 : 540;

	GLFWwindow * window = glfwCreateWindow(initial_window_size_x, initial_window_size_y, "Nova", is_full_screen ? glfwGetPrimaryMonitor() : 0, 0);
	if(!window) {
		glfwTerminate();
		return 0;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

	if(is_full_screen) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}

	glfwSetCursorPos(window, static_cast<double>(initial_window_size_x / 2) * 0.25f, static_cast<double>(initial_window_size_y / 2));

	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		return 0;
	}

	GLuint vertex_array_id;
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	GLuint const player_vert_shader_id = gl_compile_shader_from_source(BASIC_VERT_SRC, GL_VERTEX_SHADER);
	GLuint const player_frag_shader_id = gl_compile_shader_from_source(BASIC_FRAG_SRC, GL_FRAGMENT_SHADER);
	GLuint const particle_vert_shader_id = gl_compile_shader_from_source(PARTICLE_VERT_SRC, GL_VERTEX_SHADER);
	GLuint const particle_frag_shader_id = gl_compile_shader_from_source(PARTICLE_FRAG_SRC, GL_FRAGMENT_SHADER);

	GLuint const player_program_id = gl_link_shader_program(player_vert_shader_id, player_frag_shader_id);
	GLuint const particle_program_id = gl_link_shader_program(particle_vert_shader_id, particle_frag_shader_id);

	GLuint const player_position_id = glGetUniformLocation(player_program_id, "position");
	GLuint const player_radius_id = glGetUniformLocation(player_program_id, "radius");
	GLuint const player_aspect_id = glGetUniformLocation(player_program_id, "aspect");

	GLuint const particle_aspect_id = glGetUniformLocation(particle_program_id, "aspect");

	uint32_t const division_count = 64;
	float const division_step = (math::PI * 2.f) / static_cast<float>(division_count);

	uint32_t const player_vert_count = division_count * 3;
	std::vector<GLfloat> player_verts;
	player_verts.reserve(player_vert_count * 2);

	for(uint32_t i = 0; i < division_count; i++) {
		float const u = division_step * static_cast<float>(i + 0);
		float const v = division_step * static_cast<float>(i + 1);

		player_verts.push_back(std::cos(u));
		player_verts.push_back(std::sin(u));

		player_verts.push_back(std::cos(v));
		player_verts.push_back(std::sin(v));

		player_verts.push_back(0.f);
		player_verts.push_back(0.f);
	}

	VertexBuffer const player_vertex_buffer = gl_create_vertex_buffer(player_vert_count, 2, &player_verts[0], GL_STATIC_DRAW);

	float const initial_aspect_ratio = static_cast<float>(initial_window_size_x) / static_cast<float>(initial_window_size_y);

	float const initial_player_mass = 2000.f;
	float const player_particle_radius = get_particle_radius(initial_player_mass);

	uint32_t const player_count = 2;

	math::Vec2 const player_start_positions[player_count] = {
		{ -0.75f * initial_aspect_ratio, 0.f },
		{  0.75f * initial_aspect_ratio, 0.f},
	};

	Particle player_particles[player_count] = {};
	for(uint32_t i = 0; i < player_count; i++) {
		player_particles[i].position = player_start_positions[i];
		player_particles[i].velocity = math::VEC2_ZERO;
		player_particles[i].mass = initial_player_mass;
	}

	uint32_t const grid_size = 640;
	uint32_t const max_particle_count = grid_size * grid_size;
	std::vector<Particle> particles;
	particles.reserve(max_particle_count);

	float const particle_radius = 0.001805f;
	float const min_distance_sqr = (player_particle_radius + particle_radius) * (player_particle_radius + particle_radius);

	auto init_particles = [&](std::vector<Particle> & particles) {
		for(uint32_t i = 0; i < max_particle_count; i++) {
			uint32_t const x = i % grid_size;
			uint32_t const y = i / grid_size;

			float const screen_x = ((static_cast<float>(x) / static_cast<float>(grid_size)) - 0.5f) * 2.f;
			float const screen_y = ((static_cast<float>(y) / static_cast<float>(grid_size)) - 0.5f) * 2.f;

			math::Vec2 const position = { screen_x * initial_aspect_ratio, screen_y };

			particles.push_back({ position, math::VEC2_ZERO, 1.f });
		}
	};

	init_particles(particles);

	std::vector<GLfloat> particle_verts;
	particle_verts.reserve(max_particle_count * 2);
	for(uint32_t i = 0; i < max_particle_count; i++) {
		particle_verts.push_back(particles[i].position.x);
		particle_verts.push_back(particles[i].position.y);
	}

	VertexBuffer particle_vertex_buffer = gl_create_vertex_buffer(max_particle_count, 2, &particle_verts[0], GL_DYNAMIC_DRAW);

	glClearColor(0.f, 0.f, 0.f, 0.f);

	float this_frame_time = get_current_time();

	while(!glfwWindowShouldClose(window)) {
		float const last_frame_time = this_frame_time;
		this_frame_time = get_current_time();

		float const delta_time = this_frame_time - last_frame_time;

		glfwPollEvents();

		int window_size_x_int;
		int window_size_y_int;
		glfwGetWindowSize(window, &window_size_x_int, &window_size_y_int);

		float const screen_dimension_x = static_cast<float>(window_size_x_int);
		float const screen_dimension_y = static_cast<float>(window_size_y_int);

		auto reset_game = [&]() {
			particles.clear();
			init_particles(particles);

			for(uint32_t i = 0; i < player_count; i++) {
				player_particles[i].position = player_start_positions[i];
				player_particles[i].velocity = math::VEC2_ZERO;
				player_particles[i].mass = initial_player_mass;
			}

			glfwSetCursorPos(window, static_cast<double>(initial_window_size_x / 2) * 0.25f, static_cast<double>(initial_window_size_y / 2));
		};

		uint32_t active_player_count = 1;

		float const start_read_input_time = get_current_time();

		auto move_player = [&](uint32_t i, XINPUT_STATE const & controller_state) {
			float const raw_stick_x = static_cast<float>(controller_state.Gamepad.sThumbLX);
			float const raw_stick_y = static_cast<float>(controller_state.Gamepad.sThumbLY);

			float const short_max = 32767.f;
			float const stick_x = raw_stick_x / (short_max + (raw_stick_x > 0.f ? 0.f : 1.f));
			float const stick_y = raw_stick_y / (short_max + (raw_stick_y > 0.f ? 0.f : 1.f));

			math::Vec2 const mapped_stick = apply_dead_zone_mapping({ stick_x, stick_y });

			float const stick_scale_value = 2.f;
			math::Vec2 const acceleration = math::vec2_mul_float(mapped_stick, stick_scale_value);
			math::Vec2 const velocity = math::vec2_add(player_particles[i].velocity, math::vec2_mul_float(acceleration, delta_time));
			math::Vec2 const position = math::vec2_add(player_particles[i].position, math::vec2_mul_float(velocity, delta_time));

			float const damping_factor = 1.f - delta_time * 1.2f;
			player_particles[i].velocity = math::vec2_mul_float(velocity, damping_factor);
			player_particles[i].position = { math::clamp_float(position.x, -initial_aspect_ratio, initial_aspect_ratio), math::clamp_float(position.y, -1.f, 1.f ) };

			if(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_START) {
				reset_game();
			}

			active_player_count = 2;
		};

		XINPUT_STATE controller_0_state = {};
		XINPUT_STATE controller_1_state = {};

		bool const controller_0_connected = (XInputGetState(0, &controller_0_state) == ERROR_SUCCESS);
		bool const controller_1_connected = (XInputGetState(1, &controller_1_state) == ERROR_SUCCESS);

		bool const one_controller_connected = (controller_0_connected != controller_1_connected);

		if(controller_0_connected) {
			uint32_t const player_id = one_controller_connected ? 1 : 0;
			move_player(player_id, controller_0_state);
		}

		if(controller_1_connected) {
			move_player(1, controller_1_state);
		}

		// uint32_t const max_controller_count = (player_count < XUSER_MAX_COUNT) ? player_count : XUSER_MAX_COUNT;
		// for(uint32_t i = 0; i < max_controller_count; i++) {
		// 	XINPUT_STATE controller_state = {};
		// 	if(XInputGetState(i, &controller_state) == ERROR_SUCCESS) {
		// 		float const raw_stick_x = static_cast<float>(controller_state.Gamepad.sThumbLX);
		// 		float const raw_stick_y = static_cast<float>(controller_state.Gamepad.sThumbLY);

		// 		float const short_max = 32767.f;
		// 		float const stick_x = raw_stick_x / (short_max + (raw_stick_x > 0.f ? 0.f : 1.f));
		// 		float const stick_y = raw_stick_y / (short_max + (raw_stick_y > 0.f ? 0.f : 1.f));

		// 		math::Vec2 const mapped_stick = apply_dead_zone_mapping({ stick_x, stick_y });

		// 		float const stick_scale_value = 2.f;
		// 		math::Vec2 const acceleration = math::vec2_mul_float(mapped_stick, stick_scale_value);
		// 		math::Vec2 const velocity = math::vec2_add(player_particles[i].velocity, math::vec2_mul_float(acceleration, delta_time));
		// 		math::Vec2 const position = math::vec2_add(player_particles[i].position, math::vec2_mul_float(velocity, delta_time));

		// 		float const damping_factor = 1.f - delta_time * 1.2f;
		// 		player_particles[i].velocity = math::vec2_mul_float(velocity, damping_factor);
		// 		player_particles[i].position = { math::clamp_float(position.x, -initial_aspect_ratio, initial_aspect_ratio), math::clamp_float(position.y, -1.f, 1.f ) };

		// 		if(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_START) {
		// 			reset_game();
		// 		}

		// 		active_player_count = 2;
		// 	}
		// }

		float const total_read_input_time = get_current_time() - start_read_input_time;

		if(active_player_count == 1 || one_controller_connected) {
			double mouse_x;
			double mouse_y;
			glfwGetCursorPos(window, &mouse_x, &mouse_y);

			math::Vec2 const screen_coords = {
				(static_cast<float>(mouse_x) / (screen_dimension_x * 0.5f) - 1.f) * (screen_dimension_x / screen_dimension_y),
				(static_cast<float>(mouse_y) / (screen_dimension_y * 0.5f) - 1.f) * -1.f,
			};

			player_particles[0].position = math::vec2_lerp(player_particles[0].position, screen_coords, delta_time * 8.f);

			if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
				reset_game();
			}
		}

		// if(active_player_count > 1) {
		// 	math::Vec2 const direction_to_player = math::vec2_sub(player_particles[0].position, player_particles[1].position);
		// 	bool const players_touching = (math::vec2_length(direction_to_player) < player_particle_radius * 2.f);
		// 	if(players_touching) {
		// 		reset_game();
		// 	}
		// }

		float const start_particle_update_time = get_current_time();

		particle_verts.clear();

		for(uint32_t i = 0; i < particles.size(); i++) {
			math::Vec2 const current_position = particles[i].position;

			math::Vec2 gravity = { 0.f, 0.f };
			bool pause = false;

			for(uint32_t j = 0; j < active_player_count; j++)
			{
				math::Vec2 const direction_to_particle = { player_particles[j].position.x - current_position.x, player_particles[j].position.y - current_position.y };

				float const length_squared = direction_to_particle.x * direction_to_particle.x + direction_to_particle.y * direction_to_particle.y;
				if(length_squared < min_distance_sqr) {
					pause = true;
					// if(particles[i].mass > 0.f) {
					// 	player_particles[j].mass += particles[i].mass * 0.008f;	
					// 	particles[i].mass = 0.f;
					// }
				}
				else {
					float const gravity_constant = 0.00002;
					float const particle_gravity = gravity_constant * (player_particles[j].mass / length_squared);

					float const inv_length = (length_squared > 0.f) ? 1.f / std::sqrt(length_squared) : 0.f;

					gravity.x += direction_to_particle.x * inv_length * particle_gravity;
					gravity.y += direction_to_particle.y * inv_length * particle_gravity;
				}
			}

			if(!pause) {
				particles[i].velocity.x += gravity.x * delta_time;
				particles[i].velocity.y += gravity.y * delta_time;

				particles[i].position.x += particles[i].velocity.x * delta_time;
				particles[i].position.y += particles[i].velocity.y * delta_time;

				bool const out_of_bounds = (particles[i].position.x < -initial_aspect_ratio || particles[i].position.x > initial_aspect_ratio || particles[i].position.y < -1.f || particles[i].position.y > 1.f);
				if(out_of_bounds) {
					uint32_t const x = i % grid_size;
					uint32_t const y = i / grid_size;

					float const screen_x = ((static_cast<float>(x) / static_cast<float>(grid_size)) - 0.5f) * 2.f;
					float const screen_y = ((static_cast<float>(y) / static_cast<float>(grid_size)) - 0.5f) * 2.f;

					particles[i].position.x = screen_x * initial_aspect_ratio;
					particles[i].position.y = screen_y;

					particles[i].velocity = math::VEC2_ZERO;
				}
			}

			particle_verts.push_back(particles[i].position.x);
			particle_verts.push_back(particles[i].position.y);
		}

		float const total_particle_update_time = get_current_time() - start_particle_update_time;

		glBindBuffer(GL_ARRAY_BUFFER, particle_vertex_buffer.id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particle_vertex_buffer.size_in_bytes, &particle_verts[0]);

		float const aspect_ratio = screen_dimension_y / screen_dimension_x;

		glViewport(0, 0, screen_dimension_x, screen_dimension_y);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, player_vertex_buffer.id);
		glVertexAttribPointer(0, player_vertex_buffer.vert_size, GL_FLOAT, GL_FALSE, 0, static_cast<void *>(0));

		glUseProgram(player_program_id);
		glUniform1f(player_aspect_id, aspect_ratio);

		for(uint32_t i = 0; i < active_player_count; i++) {
			glUniform2f(player_position_id, player_particles[i].position.x, player_particles[i].position.y);
			glUniform1f(player_radius_id, player_particle_radius);
			glDrawArrays(GL_TRIANGLES, 0, player_vertex_buffer.vert_count);
		}

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, particle_vertex_buffer.id);
		glVertexAttribPointer(0, particle_vertex_buffer.vert_size, GL_FLOAT, GL_FALSE, 0, static_cast<void *>(0));

		glUseProgram(particle_program_id);
		glUniform1f(particle_aspect_id, aspect_ratio);
		glDrawArrays(GL_POINTS, 0, particle_vertex_buffer.vert_count);

		char txt_buffer[256] = {};
		std::sprintf(txt_buffer, "nova, dt: %.3fs, pt: %.3fs, it: %.3fs", delta_time, total_particle_update_time, total_read_input_time);
		glfwSetWindowTitle(window, txt_buffer);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}