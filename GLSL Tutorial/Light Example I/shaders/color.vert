#version 330

layout (std140) uniform Matrices {
	mat4 m_pvm;
	mat3 m_normal;
} ;

layout (std140) uniform Materials {
	vec4 diffuse;
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
	vec3 n = normalize(m_normal * normal);
	float intensity = max(dot(n, l_dir), 0.0);

	DataOut.color = intensity * diffuse;

	gl_Position = m_pvm * position ;
} 
