#version 330

in vec2 uvFragment;
layout(location = 0) out vec3 color;

uniform sampler2D textureSamplerDDS;
uniform sampler2D textureSamplerBmp;

uniform int option = 0;

void main() {
	float kernel[] = {
		1 / 16.f, 2 / 16.f, 1 / 16.f,
		2 / 16.f, 4 / 16.f, 2 / 16.f,
		1 / 16.f, 2 / 16.f, 1 / 16.f
	};

	ivec2 size = textureSize(textureSamplerDDS, 0);
	vec2 offset = 1.f / size;

	vec2 offsets[9] = vec2[](
        vec2(-offset.x,  offset.y), // top-left
        vec2( 0.0f,    offset.y), // top-center
        vec2( offset.x,  offset.y), // top-right
        vec2(-offset.x,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset.x,  0.0f),   // center-right
        vec2(-offset.x, -offset.y), // bottom-left
        vec2( 0.0f,   -offset.y), // bottom-center
        vec2( offset.x, -offset.y)  // bottom-right    
    );

	vec3 sampleTex[9];

	for(int i = 0; i < 9; ++i) {
		sampleTex[i] = texture(textureSamplerBmp, uvFragment + offsets[i]).rgb;
	}

	for(int i = 0; i < 9; ++i) {
		color += sampleTex[i] * kernel[i];
	}

	/*
	if (option == 0) {
		color = texture(textureSamplerDDS, vec2(uvFragment.s, -uvFragment.t)).rgb;
	}
	else {
		color = texture(textureSamplerBmp, uvFragment).rgb;
	}*/
}
