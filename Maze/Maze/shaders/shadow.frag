#version 330

layout(location = 0) out vec4 color;

in vec2 UV;

uniform sampler2D sampler;

void main() {
	float r = texture(sampler, UV).r;
	color = vec4(r, r, r, 1);
}
