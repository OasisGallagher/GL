#version 330

#shader vert
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec3 normal;

out VertOut {
	vec2 UV;
	vec3 worldPos;
	vec3 normal;
} vertOut;

uniform mat4 M;
uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(pos, 1);

	vertOut.UV = UV;
	vertOut.normal = normal;
	vertOut.worldPos = (M * vec4(pos, 1)).xyz;
}

#shader frag

#include "base/light.h"

in VertOut {
	vec2 UV;
	vec3 worldPos;
	vec3 normal;
} fragIn;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 bloomColor;

uniform sampler2D diffuseTex;

void main() {
	vec4 diffuse = texture(diffuseTex, vec2(fragIn.UV.s, -fragIn.UV.t));
	
	fragColor = diffuse * CalculateDirectionLight(fragIn.worldPos, fragIn.normal);

	vec3 vec = vec3(0.2126f, 0.7152f, 0.0722f);
	
	float brightness = dot(fragColor.xyz, vec);
	if(brightness > 1.f) {
		bloomColor = fragColor;
	}
	else {
		//bloomColor = fragColor;// * 0.3f;
	}
}
