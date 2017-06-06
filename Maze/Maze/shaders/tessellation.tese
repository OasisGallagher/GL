#version 410 core
layout(triangles, equal_spacing, ccw) in;

uniform mat4 gVP;
uniform sampler2D gDisplacementMap;
uniform sampler2D gColorMap;
uniform float gDispFactor;

in vec3 WorldPos_ES_in[];
in vec2 TexCoord_ES_in[];
in vec3 Normal_ES_in[];

out vec3 WorldPos_GS_in;
out vec2 TexCoord_FS_in;
out vec3 Normal_FS_in;

uniform bool test = true;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2) {
	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2) {
	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main() {
	TexCoord_FS_in = interpolate2D(TexCoord_ES_in[0], TexCoord_ES_in[1], TexCoord_ES_in[2]);
	Normal_FS_in = interpolate3D(Normal_ES_in[0], Normal_ES_in[1], Normal_ES_in[2]);
	Normal_FS_in = normalize(Normal_FS_in);

	vec3 pos = interpolate3D(WorldPos_ES_in[0], WorldPos_ES_in[1], WorldPos_ES_in[2]);
	if(!test) {
		float displacement = texture(gDisplacementMap, TexCoord_FS_in.xy).x;
		pos += Normal_FS_in * displacement * gDispFactor;
	}
	else {
		Normal_FS_in = vec3(0, 1, 0);
		float displacement = texture(gDisplacementMap, TexCoord_FS_in.xy).x;
		pos += Normal_FS_in * displacement * gDispFactor;
	}

	WorldPos_GS_in = pos;

	gl_Position = gVP * vec4(pos, 1);
}
