#version 330

in Data {
	vec2 uv;
	vec3 normal;
	vec3 worldPos;
}dataIn;

uniform sampler2D colorMap;

out vec4 fragColor;

uniform bool test = false;

void main() {
	if(test) {
		fragColor = vec4(1, 0, 0, 1);
	}
	else {
		fragColor = texture(colorMap, dataIn.uv);
	}
}