
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

uint16_t const CUSTOM_KEY_CODE_POS = GLFW_KEY_LAST + 1;

//NOTE: Use these instead of GLFW_MOUSE_BUTTON_XXX!!
uint16_t const GLFW_KEY_MOUSE_LEFT = GLFW_MOUSE_BUTTON_LEFT + CUSTOM_KEY_CODE_POS;
uint16_t const GLFW_KEY_MOUSE_RIGHT = GLFW_MOUSE_BUTTON_RIGHT + CUSTOM_KEY_CODE_POS;
uint16_t const GLFW_KEY_MOUSE_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE + CUSTOM_KEY_CODE_POS;

uint16_t const GLFW_LAST_CUSTOM_KEY = GLFW_KEY_MOUSE_MIDDLE;

uint8_t const KEY_DOWN_BIT = 0;
uint8_t const KEY_PRESSED_BIT = 1;
uint8_t const KEY_RELEASED_BIT = 2;

uint8_t const KEY_DOWN = 1 << KEY_DOWN_BIT;
uint8_t const KEY_PRESSED = 1 << KEY_PRESSED_BIT;
uint8_t const KEY_RELEASED = 1 << KEY_RELEASED_BIT;

struct KeyArray {
	uint8_t v[GLFW_LAST_CUSTOM_KEY + 1];
};

void clear_key_values(KeyArray * key_array) {	
	for(uint32_t i = 0; i < ARRAY_COUNT(key_array->v); i++) {
		key_array->v[i] = 0;
	}
}

void read_and_store_key_values(GLFWwindow * window, KeyArray * key_array) {
	for(uint32_t i = 0; i < ARRAY_COUNT(key_array->v); i++) {
		bool8_t key_was_down = key_array->v[i] & KEY_DOWN;
		bool8_t key_down = (i < CUSTOM_KEY_CODE_POS ? glfwGetKey(window, i) : glfwGetMouseButton(window, i - CUSTOM_KEY_CODE_POS)) == GLFW_PRESS;
		bool8_t key_pressed = key_down && !key_was_down;
		bool8_t key_released = !key_down && key_was_down;

		uint8_t packed_key_value = 0;
		packed_key_value |= (key_down << KEY_DOWN_BIT);
		packed_key_value |= (key_pressed << KEY_PRESSED_BIT);
		packed_key_value |= (key_released << KEY_RELEASED_BIT);	

		key_array->v[i] = packed_key_value;
	}
}

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

	//TODO: Set window position!!
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

	KeyArray key_array;
	clear_key_values(&key_array);

	float frame_time = (float)glfwGetTime();

	while(!glfwWindowShouldClose(window)) {
		float last_frame_time = frame_time;
		frame_time = (float)glfwGetTime();

		game_state.delta_time = frame_time - last_frame_time;
		game_state.total_time += game_state.delta_time;

		glfwPollEvents();
		read_and_store_key_values(window, &key_array);

		double raw_mouse_x, raw_mouse_y;
		glfwGetCursorPos(window, &raw_mouse_x, &raw_mouse_y);
		math::Vec2 mouse_pos = math::vec2((float)raw_mouse_x, (float)raw_mouse_y);
		game_state.mouse_delta = mouse_pos - game_state.mouse_pos;
		game_state.mouse_pos = mouse_pos;

		game_state.left_mouse_key_down = key_array.v[GLFW_KEY_MOUSE_LEFT] & KEY_DOWN;
		game_state.right_mouse_key_down = key_array.v[GLFW_KEY_MOUSE_RIGHT] & KEY_DOWN || key_array.v[GLFW_KEY_SPACE] & KEY_DOWN;

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