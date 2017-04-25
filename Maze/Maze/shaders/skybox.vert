#version 330

layout(location = 0) in vec3 vert;

uniform mat4 MVP;

out vec3 UV;

 void main() {
	vec4 pos = MVP * vec4(vert, 1);
	gl_Position = pos.xyww;
	UV = vert;
 }
