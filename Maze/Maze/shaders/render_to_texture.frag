#version 330

uniform float time;
uniform sampler2D textureSampler;

layout(location = 0) out vec3 fragColor;

in vec2 UV;

vec3 blur() {
	vec3 color = vec3(0);

	float kernel[] = {
		1 / 16.f, 2 / 16.f, 1 / 16.f,
		2 / 16.f, 4 / 16.f, 2 / 16.f,
		1 / 16.f, 2 / 16.f, 1 / 16.f
	};

	ivec2 size = textureSize(textureSampler, 0);
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
		sampleTex[i] = texture(textureSampler, UV + offsets[i]).rgb;
	}

	for(int i = 0; i < 9; ++i) {
		color += sampleTex[i] * kernel[i];
	}

	return color;
}

uniform bool test = true;
void main() {
	if (test) {
		fragColor = blur();//texture(textureSampler, UV).rgb;
	}
	else {
		vec2 delta = 0.005 * vec2(sin(time + 512.0 * UV.x), cos(time + 384.0 * UV.y));
		fragColor = texture(textureSampler, UV + delta).rgb;
	}
}
