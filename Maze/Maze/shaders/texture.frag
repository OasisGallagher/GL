#version 330

in vec2 uvFragment;
layout(location = 0) out vec3 color;

uniform sampler2D textureSamplerDDS;
uniform sampler2D textureSamplerBmp;

uniform int option = 0;

void main() {
	if (option == 0) {
		color = texture(textureSamplerDDS, uvFragment).rgb;
	}
	else {
		color = texture(textureSamplerBmp, uvFragment).rgb;
	}
}
