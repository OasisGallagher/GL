#version 330

in vec2 UV;

out vec3 color;

uniform sampler2D sampler;

uniform int option = 1;

void main() {
	if (option == 0) {
		color = vec3(1, 0, 0);
	}
	else {
		color = texture(sampler, vec2(UV.s, -UV.t)).rgb;
	}
}
