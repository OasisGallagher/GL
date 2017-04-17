#version 330

layout(location = 0) out vec3 color;

in vec2 UV;
in vec3 lightPosition_cameraspace;
in vec3 lightDirection_cameraspace;
in vec3 eyeDirection_cameraspace;
in vec3 normal_cameraspace;
in vec3 position_worldspace;

uniform sampler2D diffuse;
uniform sampler2D specular;
uniform vec3 lightPosition_worldspace;

void main() {
	float lightPower = 50;
	vec3 lightColor = vec3(1, 1, 1);

	float dist = length(position_worldspace - lightPosition_worldspace);
	float distSquared = dist * dist;

	vec3 diffuseColor = texture(diffuse, UV).rgb;
	vec3 specularColor = texture(specular, UV).rgb;
	vec3 ambientColor = diffuseColor * 0.1f;

	vec3 n = normalize(normal_cameraspace);
	vec3 ld = normalize(lightDirection_cameraspace); 

	float cosAlpha = max(0, dot(n, ld));

	vec3 r = reflect(-ld, n);
	vec3 e = normalize(eyeDirection_cameraspace);
	float cosTheta = max(0, dot(r, e));

	color = ambientColor
		+ cosAlpha * diffuseColor * lightColor * lightPower / distSquared
		+ pow(cosTheta, 5) * specularColor * lightColor * lightPower / distSquared;
}
