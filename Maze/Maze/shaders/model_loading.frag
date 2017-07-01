#version 330

in vec2 UV;

out vec3 fragColor;

uniform sampler2D sampler;

uniform int option = 1;

void main() {
	/*if (option == 0) {
		color = vec3(1, 0, 0);
	}
	else {
		color = texture(sampler, vec2(UV.s, -UV.t)).rgb;
	}*/

	vec3 color = vec3(0);

	float kernel[] = {
		1 / 16.f, 2 / 16.f, 1 / 16.f,
		2 / 16.f, 4 / 16.f, 2 / 16.f,
		1 / 16.f, 2 / 16.f, 1 / 16.f
	};

	ivec2 size = textureSize(sampler, 0);
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
		sampleTex[i] = texture(sampler, UV + offsets[i]).rgb;
	}

	for(int i = 0; i < 9; ++i) {
		color += sampleTex[i] * kernel[i];
	}

	fragColor = sampleTex[0];//vec3(offset, 0) ;//color;
}
