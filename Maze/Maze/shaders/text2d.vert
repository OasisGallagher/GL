#version 330

layout(location = 0) in vec2 vertex;
layout(location = 0) in vec2 vertexUV;

out vec2 UV;

void main() {
	gl_Position = vec4(vertex, 0, 1);
	UV  = vertexUV;
}