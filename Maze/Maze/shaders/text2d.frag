#version 330

layout(location = 0) out vec4 color;

in vec2 UV;

uniform sampler2D sampler;
uniform bool test = false;

void main() {
	if (!test) {
		color = texture(sampler, vec2(UV.s, -UV.t));
	}
	else {
		color = vec4(1, 0, 0, 1);
	}
}
