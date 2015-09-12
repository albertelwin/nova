
char const * SCREEN_QUAD_VERT_SRC = STRINGIFY_GLSL_SHADER(330 core,

layout(location = 0) in vec3 i_position;

out vec2 tex_coord;

void main() { 
	gl_Position = vec4(i_position, 1.0); 
	tex_coord = (i_position.xy + 1.0) * 0.5; 
}

);