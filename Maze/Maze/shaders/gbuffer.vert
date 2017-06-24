#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

uniform mat4 M;
uniform mat4 MVP;

out Data{
	vec3 pos;
	vec2 uv;
	vec3 normal;
}dataOut;

void main() {
	gl_Position = MVP * vec4(pos, 1);
	dataOut.pos = (M * vec4(pos, 1)).xyz;
	dataOut.uv = uv;
	dataOut.normal =  (M * vec4(normal, 0)).xyz;
}
