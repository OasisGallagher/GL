#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in ivec4 boneIDs;
layout(location = 4) in vec4 weights;

out Data {
	vec2 uv;
	vec4 prevPosition;
	vec4 position;
}dataOut;

const int maxBones = 100;

uniform mat4 MVP;
//uniform mat4 M;
uniform mat4 bones[maxBones];
uniform mat4 prevBones[maxBones];

uniform bool testTPos = false;

void main() {
	mat4 prevBt = mat4(0);
	mat4 bt = mat4(0);

	for(int i = 0; i < 4; ++i) {
		bt += bones[boneIDs[i]] * weights[i];
		prevBt += prevBones[boneIDs[i]] * weights[i];
	}

	if(!testTPos) {
		gl_Position = MVP * bt * vec4(position, 1);
	}
	else {
		gl_Position = MVP * vec4(position, 1);
	}

	dataOut.uv = uv;
	dataOut.prevPosition = MVP * (prevBt * vec4(position, 1));
	dataOut.position = gl_Position;
}
