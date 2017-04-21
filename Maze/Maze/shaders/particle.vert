#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in vec4 xyzs;
layout(location = 2) in vec4 color;

out vec2 UV;
out vec4 COLOR;

uniform vec3 cameraRight_worldspace;
uniform vec3 cameraUp_worldspace;
uniform mat4 VP;

void main() {
	float size = xyzs.w;
	vec3 center = xyzs.xyz;

	vec3 vert_worldspace = center + cameraRight_worldspace * vert.x * size + cameraUp_worldspace * vert.y * size;
	gl_Position = VP * vec4(vert_worldspace, 1);

	UV = vert.xy + vec2(0.5);
	COLOR = color;
}
