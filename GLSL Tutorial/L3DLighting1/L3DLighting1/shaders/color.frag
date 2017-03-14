#version 330

uniform sampler2D texUnit;

in Data {
	vec4 texCoord;
} DataIn;

out vec4 outputF;

void main()
{
	outputF = texture(texUnit, DataIn.texCoord.xy);
} 
