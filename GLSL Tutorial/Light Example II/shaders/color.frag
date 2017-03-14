#version 330

in Data {
	vec4 color;
} DataOut;

out vec4 outputF;

void main()
{
	outputF = DataOut.color;
} 
