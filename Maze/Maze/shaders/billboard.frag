#version 330

in vec2 UV;
layout(location = 0) out vec4 color;

uniform sampler2D textureSampler;

void main() {
	color = texture(textureSampler, UV);		
}
