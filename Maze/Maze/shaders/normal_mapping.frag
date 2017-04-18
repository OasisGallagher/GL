#version 330

layout(location = 0) out vec3 color;

in vec2 UV;

in vec3 lightDirection_cameraspace;
in vec3 eyeDirection_cameraspace;

in vec3 lightDirection_tangentspace;
in vec3 eyeDirection_tangentspace;

in vec3 normal_cameraspace;
in vec3 vert_worldspace;

uniform sampler2D diffuseSampler;
uniform sampler2D specularSampler;
uniform sampler2D normalSampler;

uniform vec3 lightPosition_worldspace;

uniform bool useTBN = true;
uniform bool test = false;

void main() {
	float lightPower = 40.0;
	vec3 lightColor = vec3(1, 1, 1);

	float dist = length(vert_worldspace - lightPosition_worldspace);
	float distSquared = dist * dist;

	vec3 diffuseColor = texture(diffuseSampler, UV).rgb;
	vec3 specularColor = vec3(0.3); //texture(specularSampler, UV).rgb * 0.3;

	vec3 ambientColor = diffuseColor * 0.1f;

	float cosTheta = 0, cosAlpha = 0;

	if (useTBN) {
		vec3 n = normalize(texture(normalSampler, vec2(UV.x, -UV.y)).rgb * 2.0 - 1.0);
		vec3 ld = normalize(lightDirection_tangentspace);
		cosAlpha = clamp(dot(n, ld), 0, 1);

		vec3 r = reflect(-ld, n);
		vec3 e = normalize(eyeDirection_tangentspace);
		cosTheta = clamp(dot(r, e), 0, 1);
	}
	else {
		vec3 n = normalize(normal_cameraspace);
		vec3 ld = normalize(lightDirection_cameraspace);
		cosAlpha = max(0, dot(n, ld));

		vec3 r = normalize(reflect(-ld, n));
		vec3 e = normalize(eyeDirection_cameraspace);
		cosTheta = max(0, dot(r, e));
	}

	if (test) {
		color = ambientColor;
		color = cosAlpha * diffuseColor * lightColor * lightPower / distSquared;
		//color = texture(specularSampler, UV).rgb * 0.3;
	}
	else {
		color = ambientColor
			+ cosAlpha * diffuseColor * lightColor * lightPower / distSquared
			+ pow(cosTheta, 5) * specularColor * lightColor * lightPower / distSquared;
	}
}
