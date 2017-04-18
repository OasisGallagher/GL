#version 330

layout(location = 0) out vec3 color;

in vec2 UV;

uniform sampler2D textureSampler;

void main() {
	color = texture(textureSampler, UV, -2).rgb;
}
