#version 330

layout(location = 0) out vec3 fragColor;

uniform vec3 color;

void main() {
	fragColor = color;
}