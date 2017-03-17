#version 330

layout (std140) uniform Matrices {
	mat4 m_pvm;
	mat4 m_viewModel;
	mat3 m_normal;
};

layout (std140) uniform Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	float shininess;
};

layout (std140) uniform Lights {
	vec3 l_dir;
};

in vec4 position;
in vec3 normal;

out Data {
	vec4 color;
}DataOut;

void main()
{
	vec4 spec = vec4(0);

	vec3 n = normalize(m_normal * normal);
	float intensity = max(dot(n, l_dir), 0);

	if (intensity > 0) {
		vec3 pos = vec3(m_viewModel * position);
		vec3 eye = normalize(-pos);
		// half vector.
		vec3 h = normalize(l_dir + eye);

		float intSpec = max(dot(h, n), 0);
		spec = specular * pow(intSpec, shininess);
	}

	DataOut.color = max(intensity * diffuse + spec, ambient);

	gl_Position = m_pvm * position;
}
