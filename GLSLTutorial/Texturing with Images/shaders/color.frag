#version 330
#
uniform sampler2D texUnit;
in vec4 vertexIn;
out vec4 outputF;

void main() {
	vec2 res = textureQueryLod(texUnit, vertexIn.texCoord.xy);
	outputF = vec4(texCoordV, 0, 0);
}
