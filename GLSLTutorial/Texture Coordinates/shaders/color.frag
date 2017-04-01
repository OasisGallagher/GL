#version 330

out vec4 outputF;
in vec2 texCoordV;

void main() {
	outputF = vec4(texCoordV, 0, 0);
}
