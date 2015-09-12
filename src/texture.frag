
char const * TEXTURE_FRAG_SRC = STRINGIFY_GLSL_SHADER(330 core,

in vec2 tex_coord;

layout(location = 0) out vec4 o_color;

uniform sampler2D tex_0;

void main() {
	o_color = texture(tex_0, tex_coord);
}

);