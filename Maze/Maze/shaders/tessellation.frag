#version 410 core

in vec3 WorldPos_FS_in;
in vec2 TexCoord_FS_in;
in vec3 Normal_FS_in;

uniform sampler2D gColorMap;

out vec4 fragColor;

uniform int option = 0;

void main() {
	if (TexCoord_FS_in == vec2(0)) {
	//	discard;
	//	return;
	}

	if (option == 0) {
		fragColor = texture(gColorMap, TexCoord_FS_in);
	}
	else if (option == 1) {
		fragColor = vec4(1, 0, 0, 1);
	}
}
