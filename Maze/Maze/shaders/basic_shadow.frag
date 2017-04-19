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

in vec4 ShadowCoord;

layout(location = 0) out vec3 color;

uniform sampler2D textureSampler;
uniform mat4 M;
uniform mat4 V;
uniform mat4 MVP;

uniform sampler2D shadowSampler;

uniform bool test = false;

#define USE_HALF_VECTOR

void main() {
	vec3 LightColor = vec3(1, 1, 1);
	float LightPower = 1.f;

	vec3 MaterialDiffuseColor = texture(textureSampler, UV).rgb;
	vec3 MaterialAmbientColor = vec3(0.1, 0.1, 0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3, 0.3, 0.3);
	
	vec3 n = normalize(Normal_cameraspace);
	vec3 l = normalize(LightDirection_cameraspace);

	float cosTheta = clamp(dot(n, l), 0, 1);

	vec3 E = normalize(EyeDirection_cameraspace);
	
#ifdef USE_HALF_VECTOR
	vec3 H = normalize(l + E);
	float cosAlpha = clamp(dot(n, H), 0, 1);
#else
	vec3 R = reflect(-l, n);
	float cosAlpha = clamp(dot(E, R), 0, 1);
#endif

	float shadowFactor = 1.f;
	if (ShadowCoord.z > 0.05 + texture(shadowSampler, ShadowCoord.xy).x) {
		shadowFactor = 0.5f;
	}
	
	if (!test) {
		color.rgb = MaterialAmbientColor +
			shadowFactor * MaterialDiffuseColor * LightColor * LightPower * cosTheta +
			shadowFactor * MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha, 5);
	}
	else {
		color.rgb = MaterialAmbientColor;
	}
}
