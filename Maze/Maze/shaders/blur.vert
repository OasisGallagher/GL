#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;

out VertOut {
	vec2 texCoord;
} vertOut;

void main() {
	vertOut.texCoord = texCoord;
	gl_Position = vec4(pos, 1);
}