#version 410 core

// Define the number of CPs in the output patch.
layout(vertices = 3) out;

uniform vec3 cameraPosition;

// Attributes of input CPs.
in vec3 WorldPos_tesc[];
in vec2 TexCoord_tesc[];
in vec3 Normal_tesc[];

// Attributes of output CPs.
out vec3 WorldPos_tese[];
out vec2 TexCoord_tese[];
out vec3 Normal_tese[];

float GetTessLevel(float dist0, float dist1) {
	float avgDist = (dist0 + dist1) / 2.0;
	            
	if (avgDist <= 2.0) {
		return 10.0;
	}
	else if (avgDist <= 5.0) {
		return 7.0;
	}

	return 3.0;
}

void main() {
	WorldPos_tesc[gl_InvocationID] = WorldPos_tese[gl_InvocationID];
	TexCoord_tese[gl_InvocationID] = TexCoord_tesc[gl_InvocationID];
	Normal_tesc[gl_InvocationID] = Normal_tese[gl_InvocationID];

	float distEye2Vert0 = distance(cameraPosition, WorldPos_tesc[0]);
	float distEye2Vert1 = distance(cameraPosition, WorldPos_tesc[1]);
	float distEye2Vert2 = distance(cameraPosition, WorldPos_tesc[2]);

	// Calculate the tessellation levels.
	gl_TessLevelOuter[0] = GetTessLevel(distEye2Vert1, distEye2Vert2);
	gl_TessLevelOuter[1] = GetTessLevel(distEye2Vert2, distEye2Vert0);
	gl_TessLevelOuter[2] = GetTessLevel(distEye2Vert0, distEye2Vert1);
	gl_TessLevelInner[0] = gl_TessLevelOuter[2];   
}
