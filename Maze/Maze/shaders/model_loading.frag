#version 330

in vec2 UV;

out vec3 color;

uniform sampler2D sampler;

void main() {
	color = texture(sampler, vec2(UV.s, -UV.t)).rgb;
}
