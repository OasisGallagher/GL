uniform vec3 cameraPosition;
uniform float specularPower;
uniform float materialSpecularIntensity;

layout(std140) uniform PointLight {
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
	vec3 position;
	float constant;
	float linear;
	float exp;
}pointLight;

layout(std140) uniform DirectionalLight {
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
	vec3 direction;
}directionalLight;

vec4 CalculatePointLight(vec3 worldPos, vec3 normal) {
	vec3 ld = worldPos - pointLight.position;
	float dist = length(ld);
	ld /= dist;

	vec4 ambientColor = vec4(pointLight.color * pointLight.ambientIntensity, 1);
	float diffuseFactor = dot(normal, -ld);

	vec4 diffuseColor = vec4(0);
	vec4 specularColor = vec4(0);

	if(diffuseFactor > 0) {
		diffuseColor = vec4(pointLight.color * pointLight.diffuseIntensity * diffuseFactor, 1);

		vec3 toEye = normalize(cameraPosition - worldPos);
		vec3 lightReflect = normalize(reflect(ld, normal));
		float specularFactor = dot(toEye, lightReflect);
		if(specularFactor > 0) {
			specularFactor = pow(specularFactor, specularPower);
			specularColor = vec4(pointLight.color * materialSpecularIntensity * specularFactor, 1);
		}
	}

	vec4 color = ambientColor + diffuseColor + specularColor;
	float attenuation = pointLight.constant + pointLight.linear * dist + pointLight.exp * dist * dist;
	attenuation = max(1, attenuation);
	return color / attenuation;
}

vec4 CalculateDirectionLight(vec3 worldPos, vec3 normal) {
	vec4 ambientColor = vec4(directionalLight.color * directionalLight.ambientIntensity, 1);
	float diffuseFactor = dot(normal, -directionalLight.direction);

	vec4 diffuseColor = vec4(0);
	vec4 specularColor = vec4(0);

	if(diffuseFactor > 0) {
		diffuseColor = vec4(directionalLight.color * directionalLight.diffuseIntensity * diffuseFactor, 1);

		vec3 toEye = normalize(cameraPosition - worldPos);
		vec3 lightReflect = normalize(reflect(directionalLight.direction, normal));
		float specularFactor = dot(toEye, lightReflect);
		if(specularFactor > 0) {
			specularFactor = pow(specularFactor, specularPower);
			specularColor = vec4(directionalLight.color * materialSpecularIntensity * specularFactor, 1);
		}
	}

	return ambientColor + diffuseColor + specularColor;
}
