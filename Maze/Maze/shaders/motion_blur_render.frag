#version 330

in Data {
	vec2 uv;
	vec4 prevPosition;
	vec4 position;
}dataIn;

uniform sampler2D colorMap;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 offset;
layout(location = 2) out vec3 fragColor2;

uniform bool test = false;

void main() {
	if (test) {
		fragColor = vec3(dataIn.uv, 1);
	}
	else{
		fragColor = texture(colorMap, dataIn.uv).xyz;
		fragColor2 = fragColor;
	}

	vec3 ndc = (dataIn.position / dataIn.position.w).xyz;
	vec3 prevNdc = (dataIn.prevPosition / dataIn.prevPosition.w).xyz;
	offset = (ndc - prevNdc).xy / 2;

	/*if(offset == vec2(0)) {
		fragColor = vec3(1, 0, 0);
	}
	else {
		fragColor = vec3(0, 1, 0);
	}
	*/
	//fragColor = vec3(offset, 0);
}
