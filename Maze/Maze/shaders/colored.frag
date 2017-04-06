#version 330

layout(location = 0) out vec3 color;

in vec3 fragColor;

void main() {
	color = fragColor;
}