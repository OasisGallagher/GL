#version 330
uniform int multiplicationFactor = 8;
uniform float threshold = 0.1;

in vec2 texCoordV;
out vec4 outputF;

void main() {
	vec2 t = texCoordV * multiplicationFactor;

	if (fract(t.s) < threshold || fract(t.t) < threshold) {
		outputF = vec4(texCoordV, 0, 1);
	}
	else {
		discard;
	}
}