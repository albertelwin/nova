
#ifdef WIN32
	#include <sys_win.cpp>

	#define GLEW_STATIC

	#include <GL/glew.h>
	#include <GL/glfw3.h>
#elif __APPLE__
	#include <sys_mac.cpp>

	#include <OpenGL/gl3.h>
	#include <GLFW/glfw3.h>
#endif

#include <cstdio>
#include <cstdint>
#include <ctime>

#include <gl.hpp>

#include <asset.hpp>
#include <math.hpp>
#include <nova.hpp>

#include <asset.cpp>
#include <math.cpp>
#include <nova.cpp>

void error_callback(int e, char const * desc) {
#ifdef WIN32
	if(e == GLFW_VERSION_UNAVAILABLE) {
		//TODO: Get OpenGL version
		MessageBoxA(0, "Application requires OpenGL 3.3 or higher.", "Nova", MB_OK | MB_ICONERROR | MB_TOPMOST);
	}
	else
#endif
	{
		std::printf("ERROR: %d, %s\n", e, desc);		
	}
}

void key_callback(GLFWwindow * window, int key, int scan_code, int action, int mods) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

float get_current_time() {
	return static_cast<float>(glfwGetTime());
}

math::Vec2 get_mouse_pos(GLFWwindow * window) {
	double raw_mouse_x, raw_mouse_y;
	glfwGetCursorPos(window, &raw_mouse_x, &raw_mouse_y);
	return math::vec2((float)raw_mouse_x, (float)raw_mouse_y);
}

int main() {
	glfwSetErrorCallback(error_callback);

	if(!glfwInit()) {
		return 0;
	}

	bool enable_full_screen = true;
	bool enable_v_sync = false;

	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWvidmode const * video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	uint32_t monitor_width = video_mode->width;
	uint32_t monitor_height = video_mode->height;

	uint32_t window_width = enable_full_screen ? monitor_width : 960;
	uint32_t window_height = enable_full_screen ? monitor_height : 540;

	GLFWwindow * window = glfwCreateWindow(window_width, window_height, "Nova", enable_full_screen ? glfwGetPrimaryMonitor() : 0, 0);
	if(!window) {
		glfwTerminate();
		return 0;
	}

	// uint32_t window_frame_size_lft;
	// uint32_t window_frame_size_top;
	// uint32_t window_frame_size_rgt;
	// uint32_t window_frame_size_bot;
	// glfwGetWindowFrameSize(window, &window_frame_size_lft, &window_frame_size_top, &window_frame_size_rgt, &window_frame_size_bot);

	// glfwSetWindowPos(window, window_frame_size_lft, window_frame_size_rgt);

	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glfwMakeContextCurrent(window);

#ifdef WIN32
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		return 0;
	}
#endif

	if(enable_v_sync) {
		glfwSwapInterval(1);
	}

	GLuint vertex_array_id;
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	nova::GameState game_state = {};
	game_state.back_buffer_width = window_width;
	game_state.back_buffer_height = window_height;
	game_state.key_space_pressed = false;
	game_state.key_rgt_mouse_pressed = false;
	game_state.key_lft_mouse_down = false;
	game_state.key_rgt_mouse_down = false;
	game_state.mouse_pos = get_mouse_pos(window);

	float frame_time = get_current_time();
	bool last_key_space = false;
	bool last_key_rgt_mouse = false;

	while(!glfwWindowShouldClose(window)) {
		float last_frame_time = frame_time;
		frame_time = get_current_time();

		game_state.delta_time = frame_time - last_frame_time;
		game_state.total_time += game_state.delta_time;

		glfwPollEvents();

		math::Vec2 mouse_pos = get_mouse_pos(window);
		game_state.mouse_delta = mouse_pos - game_state.mouse_pos;
		game_state.mouse_pos = mouse_pos;

		bool key_space = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
		game_state.key_space_pressed = !last_key_space && key_space;
		last_key_space = key_space;

		game_state.key_lft_mouse_down = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

		//TODO: Only use one key!!
		bool key_rgt_mouse = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
		game_state.key_rgt_mouse_pressed = (!last_key_rgt_mouse && key_rgt_mouse) || game_state.key_space_pressed;
		game_state.key_rgt_mouse_down = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) || key_space;
		last_key_rgt_mouse = key_rgt_mouse;

		nova::tick(&game_state);

		char txt_buffer[256] = {};
		std::sprintf(txt_buffer, "Nova %.3fms", game_state.delta_time * 1000.0f);
		glfwSetWindowTitle(window, txt_buffer);

		glfwSwapBuffers(window);			
	}

	glfwDestroyWindow(window);
	glfwTerminate(); 

	return 0;
}