#version 330

layout(std140) uniform DirectionalLight {
	vec3 color;					// 0
	float ambientIntensity;		// 12
	float diffuseIntensity;		// 16
	vec3 direction;				// 32
}dlight;

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

vec4 CalculateDirectionLight(vec3 worldPos, vec3 normal) {
	vec4 ambientColor = vec4(dlight.color * dlight.ambientIntensity, 1);
	float diffuseFactor = dot(normal, -dlight.direction);

	vec4 diffuseColor = vec4(0);
	vec4 specularColor = vec4(0);

	if(diffuseFactor > 0) {
		diffuseColor = vec4(dlight.color * dlight.diffuseIntensity * diffuseFactor, 1);

		vec3 toEye = normalize(cameraPosition - worldPos);
		vec3 lightReflect = normalize(reflect(dlight.direction, normal));
		float specularFactor = dot(toEye, lightReflect);
		if(specularFactor > 0) {
			specularFactor = pow(specularFactor, specularPower);
			specularColor = vec4(dlight.color * materialSpecularIntensity * specularFactor, 1);
		}
	}

	return ambientColor + diffuseColor + specularColor;
}

out vec4 fragColor;

uniform bool test = false;

void main() {
	vec2 uv = CalculateTextureCoord();
	
	if(test) {
		fragColor = vec4(texture(colorMap, uv).xyz, 1);
		return;
	}

	vec3 worldPos = texture(positionMap, uv).xyz;
	vec3 color = texture(colorMap, uv).xyz;
	vec3 normal = normalize(texture(normalMap, uv).xyz);

	fragColor = vec4(color, 1) * CalculateDirectionLight(worldPos, normal);
}
