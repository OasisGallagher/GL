#version 330

layout (std140) uniform Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	float shininess;
};

in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
} DataIn;

out vec4 outputF;

void main()
{
	vec4 spec = vec4(0);
	vec3 n = normalize(DataIn.normal);
	vec3 l = normalize(DataIn.lightDir);
	vec3 e = normalize(DataIn.eye);

	float intensity = max(dot(n, l), 0);
	if (intensity > 0) {
		// half vector.
		vec3 h = normalize(l + e);
		float intSpec = max(dot(h, n), 0);
		spec = specular * pow(intSpec, shininess);
	}

	outputF = max(intensity * diffuse + spec, ambient);
}
