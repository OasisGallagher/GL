#version 410 core

// Define the number of CPs in the output patch.
layout(vertices = 3) out;

uniform vec3 gEyeWorldPos;

// Attributes of input CPs.
in vec3 WorldPos_CS_in[];
in vec2 TexCoord_CS_in[];
in vec3 Normal_CS_in[];

// Attributes of output CPs.
out vec3 WorldPos_ES_in[];
out vec2 TexCoord_ES_in[];
out vec3 Normal_ES_in[];

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
	// We start the main function of the TCS by copying the input CP into the output CP. 
	// This function is executed once *per output CP* and the builtin variable gl_InvocationID
	// contains the index of the current invocation. 
	// The order of execution is undefined because the GPU probably distributes the CPs
	// across several of its cores and runs them in parallel.
	// We use gl_InvocationID as an index into both the input and output patch.
	TexCoord_ES_in[gl_InvocationID] = TexCoord_CS_in[gl_InvocationID];
	Normal_ES_in[gl_InvocationID] = Normal_CS_in[gl_InvocationID];
	WorldPos_ES_in[gl_InvocationID] = WorldPos_CS_in[gl_InvocationID];

	float distEye2Vert0 = distance(gEyeWorldPos, WorldPos_ES_in[0]);
	float distEye2Vert1 = distance(gEyeWorldPos, WorldPos_ES_in[1]);
	float distEye2Vert2 = distance(gEyeWorldPos, WorldPos_ES_in[2]);

	// Calculate the tessellation levels.
	// After generating the output patch we calculate the TLs.
	// The TLs can be set differently for each output patch. 
	// OpenGL provides two builtin arrays of floating points for the TLs:
	// gl_TessLevelOuter (size 4) and gl_TessLevelInner (size 2). 
	// In the case of a triangle domain we can use only the first 3 members of 
	// gl_TessLevelOuter and the first member from gl_TessLevelInner 
	// (in addition to the triangle domain there are also the quad and isoline domain that provide different access to arrays).
	// gl_TessLevelOuter[] roughly determines the number of segments on each edge
	// and gl_TessLevelInner[0] roughly determines how many rings the triangle will contain.
	gl_TessLevelOuter[0] = GetTessLevel(distEye2Vert1, distEye2Vert2);
	gl_TessLevelOuter[1] = GetTessLevel(distEye2Vert2, distEye2Vert0);
	gl_TessLevelOuter[2] = GetTessLevel(distEye2Vert0, distEye2Vert1);
	gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}
