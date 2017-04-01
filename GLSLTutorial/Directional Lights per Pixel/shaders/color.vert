#version 330

layout (std140) uniform Matrices {
	mat4 m_pvm;
	mat4 m_viewModel;
	mat3 m_normal;
};

in vec4 position;
in vec3 normal;

out Data {
	vec3 normal;
	vec4 eye;
} DataOut;

void main()
{
	DataOut.normal = normalize(m_normal * normal);
	// eye = cam - point.
	// transform point to camera space.
	DataOut.eye = -(m_viewModel * position);
	gl_Position = m_pvm * position;
}
