#version 330

layout(location = 0) out vec4 color;

in vec2 UV;

uniform sampler2D sampler;
void main() {
	color = texture(sampler, vec2(UV.s, -UV.t));
}
