#version 330

uniform mat4 pvm;

layout(location = 0) in vec4 position;
layout(location = 2) in vec2 texCoord;

out vec2 texCoordV;

void main() {
	texCoordV = texCoord;
	gl_Position = pvm * position;
}
