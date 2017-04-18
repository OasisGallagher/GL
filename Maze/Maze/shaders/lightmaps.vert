#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 vertUV;

out vec2 UV;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(vert, 1);
	UV = vertUV;
}

