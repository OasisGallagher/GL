#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 VP;
uniform vec3 cameraPosition;
uniform float size = 0.1;

out vec2 UV;

void main() {
	vec3 vert = gl_in[0].gl_Position.xyz;
	vec3 fwd = normalize(cameraPosition - vert);
	vec3 up = vec3(0, 1, 0);
	vec3 right = cross(up, fwd);
	up = cross(fwd, right);

	// left bottom.
	gl_Position = VP * vec4(vert + (-size * right) + (-size * up), 1);
	UV = vec2(0, 0);
	EmitVertex();

	// top left.
	gl_Position = VP * vec4(vert + (-size * right) + (size * up), 1);
	UV = vec2(0, 1);
	EmitVertex();

	// right bottom.
	gl_Position = VP * vec4(vert + (size * right) + (-size * up), 1);
	UV = vec2(1, 0);
	EmitVertex();

	// top right.
	gl_Position = VP * vec4(vert + (size * right) + (size * up), 1);
	UV = vec2(1, 1);
	EmitVertex();

	EndPrimitive();
}
