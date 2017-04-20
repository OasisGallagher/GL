#version 330

layout(location = 0) in vec3 vert_modelspace;

out vec2 UV;

uniform vec2 billboardSize;
uniform vec3 billboardPosition;

uniform vec3 cameraUp_worldspace;
uniform vec3 cameraRight_worldspace;

uniform mat4 VP;

uniform bool fixSize = true;

void main() {
	UV = vert_modelspace.xy + vec2(0.5);

	if (!fixSize) {
		vec3 vert_worldspace = billboardPosition
			+ (cameraUp_worldspace * vert_modelspace.y * billboardSize.y)
			+ (cameraRight_worldspace * vert_modelspace.x * billboardSize.x);

		gl_Position = VP * vec4(vert_worldspace, 1);
	}
	else {
		vec4 vert_screenspace = VP * vec4(billboardPosition, 1);
		vert_screenspace /= vert_screenspace.w;
		vert_screenspace.xy += vert_modelspace.xy * vec2(0.2, 0.05);

		gl_Position = vert_screenspace;
	}
}
