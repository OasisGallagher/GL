#version 330

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 color;

out vec3 fragColor;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(vertex, 1);
	fragColor = color;
}