#version 330
out vec4 fragColor;

in VertOut {
	vec2 texCoord;
} fragIn;

uniform sampler2D source;

uniform int horizontal;

uniform float weights[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);
/*
const float weights[] = {
	0.2270270270f, 0.1945945946f, 0.1216216216f, 0.0540540541f, 0.0162162162f
};
*/
uniform bool test = false;

void main() {
	vec2 offset = 1.f / textureSize(source, 0);
	vec3 result = texture(source, fragIn.texCoord).rgb * weights[0];

	if(!test){
		if(horizontal != 0) {
			for(int i = 1; i < weights.length(); ++i) {
				result += texture(source, fragIn.texCoord + vec2(offset.x * i, 0)).rgb * weights[i];
				result += texture(source, fragIn.texCoord - vec2(offset.x * i, 0)).rgb * weights[i];
			}
		}
		else {
			for(int i = 1; i < weights.length(); ++i) {
				result += texture(source, fragIn.texCoord + vec2(0, offset.y * i)).rgb * weights[i];
				result += texture(source, fragIn.texCoord - vec2(0, offset.y * i)).rgb * weights[i];
			}
		}
	}

	fragColor =  vec4(result, 1);
}