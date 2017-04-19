#version 330

layout(location = 0) in vec3 vert;

uniform mat4 depthMVP;

void main() {
	gl_Position = depthMVP * vec4(vert, 1);
}
