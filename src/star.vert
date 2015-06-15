
char const * STAR_VERT_SRC = STRINGIFY_GLSL_SHADER(330 core,

layout(location = 0) in vec3 i_position;
//layout(location = 1) in vec2 i_tex_coord;

uniform mat4 world_matrix = mat4(1.0);
uniform mat4 world_view_projection_matrix = mat4(1.0);

out vec3 local_pos;
out vec3 world_normal;

void main() {
	local_pos = i_position;
	world_normal = (world_matrix * vec4(i_position, 0.0)).xyz;
	gl_Position = world_view_projection_matrix * vec4(i_position, 1.0);
}

);