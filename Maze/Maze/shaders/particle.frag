#version 330

in vec2 UV;
in vec4 COLOR;

layout(location = 0)out vec4 fragColor;
uniform sampler2D textureSampler;

uniform bool test = false;

void main() {
	if (!test) {
		fragColor = texture(textureSampler, UV) * COLOR;
	}
	else {
		fragColor = vec4(UV, 0, 1);
	}
}
