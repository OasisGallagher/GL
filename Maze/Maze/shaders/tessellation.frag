#version 410 core

in vec3 WorldPos_frag;
in vec2 TexCoord_frag;
in vec3 Normal_frag;

uniform sampler2D colorMap;

out vec4 fragColor;

uniform int option = 1;

void main() {
	if (option == 0) {
		fragColor = texture(colorMap, TexCoord_frag);
	}
	else {
		fragColor = vec4(1, 0, 0, 1);
	}
}
