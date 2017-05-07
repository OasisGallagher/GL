#version 330

uniform sampler2D textureSampler;

uniform int discardOption = 0;

in vec2 UV;

out vec4 color;

void main() {
	color = texture(textureSampler, vec2(UV.s, UV.t));
	if(discardOption == 0) {
		if (color.r == 0 && color.g == 0 && color.b == 0) {
			discard;
		}
	}
	else if(discardOption == 1) {
		if (color.r >= 0.9 && color.g >= 0.9 && color.b >= 0.9) {
			discard;
		}
	}
}
