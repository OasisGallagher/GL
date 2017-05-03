#version 330

uniform sampler2D textureSampler;

in vec2 UV;

out vec4 color;

void main() {
	color = texture(textureSampler, vec2(UV.s, 1 - UV.t));
	if(color.r == 0 && color.g == 0 && color.b == 0) {
		discard;
	}
}
