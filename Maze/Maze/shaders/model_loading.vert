#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec3 normal;

out VertOut {
	vec2 UV;
	vec3 worldPos;
	vec3 normal;
} vertOut;

uniform mat4 M;
uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(pos, 1);

	vertOut.UV = UV;
	vertOut.normal = normal;
	vertOut.worldPos = (M * vec4(pos, 1)).xyz;
}
