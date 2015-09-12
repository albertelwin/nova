
char const * THRESHOLD_TEXTURE_FRAG_SRC = STRINGIFY_GLSL_SHADER(330 core,

in vec2 tex_coord;

layout(location = 0) out vec4 o_color;

uniform sampler2D tex_0;

uniform float bias = 0.0;
uniform float scale = 1.0;

void main() {
	o_color = max(vec4(0.0), texture(tex_0, tex_coord) + bias) * scale;
}

);