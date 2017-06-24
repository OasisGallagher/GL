#version 330

layout(std140) uniform PointLight {
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
	vec3 position;
	float constant;
	float linear;
	float exp;
}plight;

uniform vec2 screenSize;
uniform vec3 cameraPosition;
uniform float specularPower;
uniform float materialSpecularIntensity;

uniform sampler2D positionMap;
uniform sampler2D colorMap;
uniform sampler2D normalMap;

vec2 CalculateTextureCoord() {
	return gl_FragCoord.xy / screenSize;
}

vec4 CalculatePointLight(vec3 worldPos, vec3 normal) {
	vec3 ld = worldPos - plight.position;
	float dist = length(ld);
	ld /= dist;

	vec4 ambientColor = vec4(plight.color * plight.ambientIntensity, 1);
	float diffuseFactor = dot(normal, -ld);

	vec4 diffuseColor = vec4(0);
	vec4 specularColor = vec4(0);

	if(diffuseFactor > 0) {
		diffuseColor = vec4(plight.color * plight.diffuseIntensity * diffuseFactor, 1);

		vec3 toEye = normalize(cameraPosition - worldPos);
		vec3 lightReflect = normalize(reflect(ld, normal));
		float specularFactor = dot(toEye, lightReflect);
		if(specularFactor > 0) {
			specularFactor = pow(specularFactor, specularPower);
			specularColor = vec4(plight.color * materialSpecularIntensity * specularFactor, 1);
		}
	}

	vec4 color = ambientColor + diffuseColor + specularColor;
	float attenuation = plight.constant + plight.linear * dist + plight.exp * dist * dist;
	attenuation = max(1, attenuation);
	return color / attenuation;
}

out vec4 fragColor;

uniform bool test = true;

void main() {
	vec2 uv = CalculateTextureCoord();
	
	vec3 worldPos = texture(positionMap, uv).xyz;
	vec3 color = texture(colorMap, uv).xyz;
	vec3 normal = normalize(texture(normalMap, uv).xyz);

	fragColor = vec4(color, 1) * CalculatePointLight(worldPos, normal);
}
