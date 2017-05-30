#version 410 core
layout(triangles, equal_spacing, ccw) in;

uniform mat4 VP;
uniform sampler2D displacementMap;
uniform float displacementFactor;

in vec3 WorldPos_tese[];
in vec2 TexCoord_tese[];
in vec3 Normal_tese[];

out vec3 WorldPos_frag;
out vec2 TexCoord_frag;
out vec3 Normal_frag;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2) {
	return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2) {
	return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

void main() {
	TexCoord_frag = interpolate2D(TexCoord_tese[0], TexCoord_tese[1], TexCoord_tese[2]);
	Normal_frag = interpolate3D(Normal_tese[0], Normal_tese[1], Normal_tese[2]);
	Normal_frag = normalize(Normal_frag);

	WorldPos_frag = interpolate3D(WorldPos_tese[0], WorldPos_tese[1], WorldPos_tese[2]);
	float displacement = texture(displacementMap, TexCoord_tese).x;
	WorldPos_frag += Normal_frag * displacement * displacementFactor;
	gl_Position = VP * vec4(WorldPos_frag, 1);
}
