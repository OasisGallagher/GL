#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in ivec4 boneIDs;
layout(location = 4) in vec4 weights;

out Data {
	vec2 uv;
	vec3 normal;
	vec3 worldPos;
}dataOut;

const int maxBones = 100;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 bones[maxBones];

void main() {
	mat4 bt = mat4(0);

	for(int i = 0; i < 4; ++i) {
		bt += bones[boneIDs[i]] * weights[i];
	}

	vec4 localPosition = bt * vec4(position, 1);
	gl_Position = MVP * localPosition;

	dataOut.uv = uv;

	vec4 localNormal = bt * vec4(normal, 0);
	dataOut.normal = (M * localNormal).xyz;
	dataOut.worldPos = (M * localPosition).xyz;
}
