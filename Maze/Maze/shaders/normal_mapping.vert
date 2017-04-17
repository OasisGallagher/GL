#version 330

uniform mat4 M = mat4(1);
uniform mat4 V;
uniform mat4 MVP;
uniform vec3 lightPosition_worldspace;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 vertexUV;
layout(location = 3) in vec3 normal;

out vec2 UV;
out vec3 lightPosition_cameraspace;
out vec3 lightDirection_cameraspace;
out vec3 eyeDirection_cameraspace;
out vec3 normal_cameraspace;
out vec3 position_worldspace;

void main() {
	gl_Position = MVP * vec4(vertex, 1);

	position_worldspace = (M * vec4(vertex, 1)).xyz;
	
	vec3 vertex_cameraspace = (V * M * vec4(vertex, 1)).xyz;

	lightPosition_cameraspace = (V * vec4(lightPosition_worldspace, 1)).xyz;
	lightDirection_cameraspace = lightPosition_cameraspace - vertex_cameraspace;

	eyeDirection_cameraspace = vec3(0) - vertex_cameraspace;

	normal_cameraspace = (V * vec4(normal, 0)).xyz;

	UV = vertexUV;
}
