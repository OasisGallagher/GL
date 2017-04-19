#version 330
layout(location = 0) in vec3 vert;

out vec2 UV;

void main() {
	gl_Position = vec4(vert, 1);
	UV = (vert.xy + 1) / 2;
}
