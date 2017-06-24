#version 330

in Data {
	vec3 pos;	
	vec2 uv;
	vec3 normal;
} dataIn;

layout(location = 0) out vec3 pos;
layout(location = 1) out vec3 diffuse;
layout(location = 2) out vec3 normal;

uniform sampler2D sampler;
uniform bool test = false;

void main() {
	pos = dataIn.pos;
	if (!test) {
		diffuse = texture(sampler, dataIn.uv).rgb;
	}
	else {
		diffuse = vec3(dataIn.uv, 1);
	}

	normal = normalize(dataIn.normal);
}
