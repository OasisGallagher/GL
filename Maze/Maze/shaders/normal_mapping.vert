#version 330

uniform mat4 M = mat4(1);
uniform mat4 V;
uniform mat4 MVP;
uniform vec3 lightPosition_worldspace;

layout(location = 0) in vec3 vert_modelspace;
layout(location = 1) in vec2 uv_modelspace;
layout(location = 2) in vec3 normal_modelspace;
layout(location = 3) in vec3 tangent_modelspace;
layout(location = 4) in vec3 bitangent_modelspace;

out vec2 UV;

out vec3 lightDirection_cameraspace;
out vec3 eyeDirection_cameraspace;

out vec3 lightDirection_tangentspace;
out vec3 eyeDirection_tangentspace;

out vec3 normal_cameraspace;
out vec3 vert_worldspace;

uniform bool dummy = false;

void main() {
	UV = uv_modelspace;
	gl_Position = MVP * vec4(vert_modelspace, 1);
	
	normal_cameraspace = (V * M * vec4(normal_modelspace, 0)).xyz;
	vert_worldspace = (M * vec4(vert_modelspace, 1)).xyz;

	vec3 tangent_cameraspace = (V * M * vec4(tangent_modelspace, 0)).xyz;
	vec3 bitangent_cameraspace = (V * M * vec4(bitangent_modelspace, 0)).xyz;

	mat3 TBN = transpose(mat3(tangent_cameraspace, bitangent_cameraspace, normal_cameraspace));

	vec3 vert_cameraspace = (V * vec4(vert_worldspace, 1)).xyz;

	vec3 lightPosition_cameraspace = (V * vec4(lightPosition_worldspace, 1)).xyz;
	lightDirection_cameraspace = lightPosition_cameraspace - vert_cameraspace;

	eyeDirection_cameraspace = vec3(0) - vert_cameraspace;

	lightDirection_tangentspace = TBN * lightDirection_cameraspace;
	eyeDirection_tangentspace = TBN * eyeDirection_cameraspace;
}
