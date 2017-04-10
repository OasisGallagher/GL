#version 330

in vec2 UV;

out vec3 color;

uniform sampler2D uvSampler;

void main() {
	color = texture(uvSampler, UV).rgb;
}
