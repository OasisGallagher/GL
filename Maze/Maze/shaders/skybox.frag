#version 330

layout(location = 0) out vec4 color;

in vec3 UV;

uniform samplerCube textureSampler;

void main() {
	color = texture(textureSampler, UV);	
}
