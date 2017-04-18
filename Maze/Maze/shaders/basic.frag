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

layout(location = 0) out vec4 color;

uniform sampler2D textureSampler;
uniform mat4 M;
uniform mat4 V;
uniform mat4 MVP;
uniform vec3 LightPosition_worldspace;

uniform float CutOff = cos(15 * 3.14159f / 180.f);

#define USE_HALF_VECTOR

void main() {
	vec3 LightColor = vec3(1, 1, 1);
	float LightPower = 50.f;

	vec3 MaterialDiffuseColor = texture(textureSampler, UV).rgb;
	vec3 MaterialAmbientColor = vec3(0.1, 0.1, 0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3, 0.3, 0.3);

	float dist = length(LightPosition_worldspace - Position_worldspace);
	
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
	
	float distSquared = (dist * dist);
	
	color.rgb = MaterialAmbientColor +
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / distSquared +
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha, 5) / distSquared;

	color.a = 0.3;
}
