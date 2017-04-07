#version 330

in vec2 uvFragment;
layout(location = 0) out vec3 color;

uniform sampler2D textureSamplerDDS;
uniform sampler2D textureSamplerBmp;

void main() {
	color = texture(textureSamplerDDS, uvFragment).rgb;
	color = texture(textureSamplerBmp, uvFragment).rgb;
}
