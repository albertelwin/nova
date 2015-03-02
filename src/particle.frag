
char const * PARTICLE_FRAG_SRC = STRINGIFY_GLSL_SHADER(330 core,

layout(location = 0) out vec4 o_color;

uniform float total_frame_count = 1.0;

void main() {
	// float d = 1.0 - log(distance(gl_PointCoord - 0.5, vec2(0.0)) * 8.0);
	// o_color = vec4(vec3(d), 0.04);
	// o_color = vec4(vec3(1.0), 1.0 / 256.0);
	o_color = vec4(vec3(1.0), 0.2);
}

);