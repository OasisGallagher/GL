#version 330

layout (std140) uniform Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	float shininess;
	vec4 color;
};

layout (std140) uniform Lights {
	vec4 l_pos;
	vec4 l_spotDir;
	float l_spotCutOff;
};

in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
} DataIn;

out vec4 outputF;

void main()
{
	float intensity = 0;
	vec4 spec = vec4(0);

	vec3 ld = normalize(DataIn.lightDir);
	vec3 sd = normalize(vec3(-l_spotDir));

	// inside the cone.
	if (dot(sd, ld) > l_spotCutOff) {
		vec3 n = normalize(DataIn.normal);
		intensity = max(dot(n, ld), 0);

		if(intensity > 0) {
			vec3 eye = normalize(DataIn.eye);
			vec3 h = normalize(ld + eye);
			float intSpec = max(dot(h, n), 0);
			spec = specular * pow(intSpec, shininess);
		}
	}

	outputF = color * max(intensity * diffuse + spec, ambient);
}
