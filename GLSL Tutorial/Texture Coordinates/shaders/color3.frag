#version 330

uniform int option = 1;

uniform vec4 color1 = vec4(0, 0, 1, 1);
uniform vec4 color2 = vec4(1, 1, 0.5, 1);

uniform int mulFactor = 8;

in vec2 texCoordV;
out vec4 outputF;

void computeOutput() {
	if(fract(texCoordV.s * mulFactor) < 0.5) {
		outputF = color1;
	}
	else {
		outputF = color2;
	}
}

void computeOutputSmooth() {
	vec2 t = texCoordV * mulFactor;
	float f = fract(t.s);
	if (f < 0.4) {
		outputF = color1;
	}
	else if(f < 0.5) {
		outputF = mix(color1, color2, 10 * (f - 0.4));
	}
	else if(f < 0.9) {
		outputF = color2;
	}
	else {
		outputF = mix(color2, color1, 10 * (f - 0.9));
	}
}

void computeOutputSmoothStep() {
	float x = fract(texCoordV.s * mulFactor);
	float f = smoothstep(0.4, 0.5, x) - smoothstep(0.9, 1, x);
	outputF = mix(color2, color1, f);
}

void main() {
	if (option == 0) {
		computeOutput();
	}
	else if(option == 1) {
		computeOutputSmooth();
	}
	else {
		computeOutputSmoothStep();
	}
}

