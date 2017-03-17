#version 330

layout (std140) uniform Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	float shininess;
};

layout (std140) uniform Lights {
	// camera space.
	vec3 l_dir;
};

in Data {
	vec3 normal;
	vec4 eye;
} DataIn;

out vec4 outputF;

void main()
{
	vec4 spec = vec4(0);

	vec3 n = normalize(DataIn.normal);
	vec3 e = normalize(vec3(DataIn.eye));
	
	float intensity = max(dot(n, l_dir), 0);

	if (intensity > 0) {
		// half vector.
		vec3 h = normalize(l_dir + e);
		
		// compute the specular term into spec.
		float intSpec = max(dot(h, n), 0);
		spec = specular * pow(intSpec, shininess);
	}

	outputF = max(intensity * diffuse + spec, ambient);
}
