#version 330

layout(location = 0) out vec3 color;

in vec3 fragColor;

uniform bool test = true;

void main() {
	if (test) {
		color = vec3(1, 0, 0);
	}
	else {
		color = fragColor;
	}
}