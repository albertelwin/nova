
char const * BASIC_FRAG_SRC = STRINGIFY_GLSL_SHADER(330 core,

layout(location = 0) out vec4 o_color;

uniform vec3 color = vec3(0.0);

void main() {
	o_color = vec4(color, 1.0);
}

);