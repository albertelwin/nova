
char const * BASIC_VERT_SRC = STRINGIFY_GLSL_SHADER(330 core,

layout(location = 0) in vec2 i_position;

uniform float aspect = 1.0;

uniform vec2 position;
uniform float radius;

void main() {
	vec2 world_pos = i_position * radius + position;
	gl_Position = vec4(world_pos.x * aspect, world_pos.y, 0.0, 1.0);
}

);