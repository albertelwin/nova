
char const * PARTICLE_FRAG_SRC = STRINGIFY_GLSL_SHADER(330 core,

layout(location = 0) out vec4 o_color;

void main() {
	o_color = vec4(vec3(1.0), 0.2);
}

);