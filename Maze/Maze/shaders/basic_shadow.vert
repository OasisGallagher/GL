#version 330

// vertex position, in model space.
layout(location = 0) in vec3 vertexPosition_modelspace;

// vertex UV.
layout(location = 1) in vec2 vertexUV;

// vertex normal, in model space.
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec2 UV;

// vertex position, in world space.
out vec3 Position_worldspace;

// normal vector, in camera space.
out vec3 Normal_cameraspace;

// vector that goes from the vertex to camera, in camera space.
out vec3 EyeDirection_cameraspace;

// vector that goes from the vertex to light, in camera space.
out vec3 LightDirection_cameraspace;

out vec4 ShadowCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 MVP;
uniform mat4 ShadowMVP;
uniform vec3 LightInvDirection_worldspace;

void main() {
	gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
	
	ShadowCoord = ShadowMVP * vec4(vertexPosition_modelspace, 1);

	Position_worldspace = (M * vec4(vertexPosition_modelspace, 1)).xyz;

	vec3 vertexPosition_cameraspace = (V * vec4(Position_worldspace, 1)).xyz;
	EyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;

	LightDirection_cameraspace = (V * vec4(LightInvDirection_worldspace, 0)).xyz;

	// top-left 3x3 matrix * normal.
	// correct only if M does not scale the model.
	// use its inverse transpose if not.
	Normal_cameraspace = (V * M * vec4(vertexNormal_modelspace, 0)).xyz;

	UV = vertexUV;
}
