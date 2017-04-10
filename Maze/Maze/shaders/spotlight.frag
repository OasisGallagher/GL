#version 330

in vec2 UV;

// vertex position, in world space.
in vec3 Position_worldspace;

// normal vector, in camera space.
in vec3 Normal_cameraspace;

// vector that goes from the vertex to camera, in camera space.
in vec3 EyeDirection_cameraspace;

// vector that goes from the vertex to the light, in camera space.
in vec3 LightDirection_cameraspace;

out vec3 color;

uniform sampler2D textureSampler;
uniform mat4 M;
uniform mat4 V;
uniform mat4 MVP;
uniform vec3 LightPosition_worldspace;

uniform float CutOff = cos(15 * 3.14 / 180);

void main() {
	vec3 LightColor = vec3(1, 1, 1);
	float LightPower = 50.f;

	vec3 MaterialDiffuseColor = texture(textureSampler, UV).rgb;
	vec3 MaterialAmbientColor = vec3(0.1, 0.1, 0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3, 0.3, 0.3);

	vec3 LightPosition_cameraspace = (V * vec4(LightPosition_worldspace, 1)).xyz;
	float cosBeta = dot(LightPosition_cameraspace, -LightDirection_cameraspace);

	float dist = length(LightPosition_worldspace - Position_worldspace);
	float distSquared = dist * dist;

	vec3 spec = vec3(0);

	if (cosBeta > CutOff) {
		float intensity = max(dot(Normal_cameraspace, LightDirection_cameraspace), 0);

		if (intensity > 0) {
			vec3 h = (EyeDirection_cameraspace + LightDirection_cameraspace);
			spec = MaterialSpecularColor * LightColor * LightPower * pow(max(dot(h, Normal_cameraspace), 0), 5) / distSquared;
		}
	}

	float cosTheta = max(dot(Normal_cameraspace, LightDirection_cameraspace), 0);
	color = spec + MaterialAmbientColor;// + MaterialDiffuseColor * LightColor * LightPower * cosTheta / distSquared;
}
