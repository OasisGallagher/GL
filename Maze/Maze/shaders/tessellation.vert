#version 410 core

layout(location = 0) in vec3 Position_vert;
layout(location = 1) in vec2 TexCoord_vert;
layout(location = 2) in vec3 Normal_vert;

uniform mat4 M;

out vec3 WorldPos_tesc;
out vec2 TexCoord_tesc;
out vec3 Normal_tesc;

void main() {

	WorldPos_tesc = (M * vec4(Position_vert, 1)).xyz;
	TexCoord_tesc = TexCoord_vert;
	Normal_tesc = (M * vec4(Normal_vert, 1)).xyz;
}
