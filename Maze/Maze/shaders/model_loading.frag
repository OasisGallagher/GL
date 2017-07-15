#version 330

in VertOut {
	vec2 UV;
	vec3 worldPos;
	vec3 normal;
} fragIn;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 bloomColor;

layout(std140) uniform DirectionalLight {
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
	vec3 direction;
} dl;

uniform sampler2D sampler;
uniform vec3 cameraPosition;

const float specularPower = 0.2f;
const float materialSpecularIntensity = 4.3f;

vec4 calculateLight() {
	vec4 ambient = vec4(dl.color * dl.ambientIntensity, 1);
	float diffuseFactor = dot(fragIn.normal, -dl.direction);

	vec4 diffuseColor = vec4(0);
	vec4 specularColor = vec4(0);

	if(diffuseFactor > 0) {
		diffuseColor = vec4(dl.color * dl.diffuseIntensity * diffuseFactor, 1);

		vec3 toEye = normalize(cameraPosition - fragIn.worldPos);
		vec3 lightReflect = normalize(reflect(dl.direction, fragIn.normal));

		float specularFactor = dot(toEye, lightReflect);
		if (specularFactor > 0) {
			specularFactor = pow(specularFactor, specularPower);
			specularColor = vec4(dl.color * materialSpecularIntensity * specularFactor, 1);
		}
	}

	return diffuseColor + specularColor;
}

void main() {
	vec4 diffuse = texture(sampler, vec2(fragIn.UV.s, -fragIn.UV.t));
	
	fragColor = diffuse * calculateLight();

	vec3 vec = vec3(0.2126f, 0.7152f, 0.0722f);
	
	float brightness = dot(fragColor.xyz, vec);
	if(brightness > 1.f) {
		bloomColor = fragColor;
	}
	else {
		//bloomColor = fragColor;// * 0.3f;
	}
}
