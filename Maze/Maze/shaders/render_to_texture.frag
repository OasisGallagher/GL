#version 330

uniform float time;
uniform sampler2D textureSampler;

layout(location = 0) out vec3 color;

in vec2 UV;

void main() {
	vec2 delta = 0.005 * vec2(sin(time + 512.0 * UV.x), cos(time + 384.0 * UV.y));
	color = texture(textureSampler, UV + delta).rgb;
}
