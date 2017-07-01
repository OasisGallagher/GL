#version 330

in Data {
	vec2 uv;
} dataIn;

out vec4 fragColor;

uniform sampler2D colorMap;
uniform sampler2D motionBlurMap;
uniform sampler2D prevTexture;

vec3 blur() {
	vec2 UV = dataIn.uv;
	vec3 color = vec3(0);

	float kernel[] = {
		1 / 16.f, 2 / 16.f, 1 / 16.f,
		2 / 16.f, 4 / 16.f, 2 / 16.f,
		1 / 16.f, 2 / 16.f, 1 / 16.f
	};

	ivec2 size = textureSize(colorMap, 0);
	vec2 offset = 1.f / vec2(300.f); //size;

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
		sampleTex[i] = texture(colorMap, UV + offsets[i]).rgb;
	}

	for(int i = 0; i < 9; ++i) {
		color += sampleTex[i] * kernel[i];
	}

	return color;
}

uniform bool test = false;

void main() {
	vec2 motion = texture(motionBlurMap, dataIn.uv).xy;

	vec2 uv = dataIn.uv;
	vec4 color = texture(colorMap, uv);

	uv += motion;

	const int numSamples = 5;
	for(int i = 1; i < numSamples; ++i, uv += motion) {
		color += texture(colorMap, uv);
	}

	color /= numSamples;
	color.a = 1;

	if(test) {
		vec4 prev = texture(prevTexture, dataIn.uv);
		vec4 current = texture(colorMap, dataIn.uv);
		if (prev == current) {
			fragColor = vec4(0);//1, 0, 0, 1);
		}
		else {
			fragColor = abs(current - prev); //vec4(0, 1, 0, 1);
		}
		fragColor = mix(prev, current, 0.1);
	}
	else {
		fragColor = color;
	}
}
