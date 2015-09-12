
char const * BLUR_TEXTURE_FRAG_SRC = STRINGIFY_GLSL_SHADER(330 core,

in vec2 tex_coord;

layout(location = 0) out vec4 o_color;

uniform sampler2D tex_0;

uniform float tex_height_r;

uniform float offset[3] = float[](         0.0, 1.3846153846, 3.2307692308);
uniform float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main() {
	o_color = texture(tex_0, tex_coord) * weight[0];
	for(int i = 1; i < 3; i++) {
		o_color += texture(tex_0, tex_coord + vec2(0.0, offset[i] * tex_height_r)) * weight[i];
		o_color += texture(tex_0, tex_coord - vec2(0.0, offset[i] * tex_height_r)) * weight[i];
	}
}

);