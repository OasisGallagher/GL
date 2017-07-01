#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

out Data {
	vec2 uv;
}dataOut;

void main() {
	gl_Position = vec4(position, 1); 
	dataOut.uv = uv;
}
