#version 330

#shader vert

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;

out VertOut {
	vec2 texCoord;
} vertOut;

void main() {
	vertOut.texCoord = texCoord;
	gl_Position = vec4(pos, 1);
}

#shader frag

out vec4 fragColor;

in VertOut {
	vec2 texCoord;
} fragIn;

uniform sampler2D source;

void main() {
	fragColor = texture(source, fragIn.texCoord) * 0.3f;
}


