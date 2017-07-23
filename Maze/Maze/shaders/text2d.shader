#version 330

#shader vert

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 texCoord;

out vec2 UV;

void main() {
	gl_Position = vec4(vertex, 0, 1);
	UV  = texCoord;
}

#shader frag

layout(location = 0) out vec4 fragColor;

in vec2 UV;

uniform sampler2D tex;
uniform vec3 color;
uniform bool test = false;

void main() {
	if (!test) {
		fragColor = texture(tex, vec2(UV.s, UV.t));
		if(fragColor == vec4(1)) {
			fragColor = vec4(1, 0, 0);
		}
	}
	else {
		fragColor = vec4(1, 0, 0, 1) * color;
	}
}
