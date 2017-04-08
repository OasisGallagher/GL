#include "input.h"
#include "shader.h"
#include "texture.h"
#include "example.h"

static const float defaultAspect = 4.f / 3.f;

Example::Example() {
	shader_ = new Shader;
}

Example::~Example() {
	delete shader_;
}

void Example::GetEnvRequirement(AppEnv& env) {
	env.depthTest = true;
	env.backgroundColor = glm::vec4(0.0f, 0.0f, 0.4f, 0.0f);
}

Example_RedTriangle::Example_RedTriangle() {
	const GLfloat data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);
	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	shader_->Load(ShaderTypeVertex, "shaders/simple.vert");
	shader_->Load(ShaderTypeFragment, "shaders/simple.frag");
	shader_->Link();
}

void Example_RedTriangle::Update(float deltaTime) {
	shader_->Use();

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);
}

void Example_RedTriangle::GetEnvRequirement(AppEnv& env) {
	Example::GetEnvRequirement(env);
	env.depthTest = false;
}

Example_RedTriangle::~Example_RedTriangle() {
	glDeleteBuffers(1, &vbo_);
	glDeleteVertexArrays(1, &vao_);
}

Example_Matrices::Example_Matrices() {
	glm::mat4 proj = glm::perspective(45.f, defaultAspect, 0.1f, 100.f);
	glm::mat4 view = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 model = glm::mat4(1.f);
	mvp_ = proj * view * model;

	GLfloat vertexBufferData[] = {
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		0.f, 1.f, 0.f
	};

	GLushort elementBufferData[] = {
		0, 1, 2
	};

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

	shader_->Load(ShaderTypeVertex, "shaders/simple_transform.vert");
	shader_->Load(ShaderTypeFragment, "shaders/simple_transform.frag");
	shader_->Link();

	shader_->Use();
	shader_->SetUniform("MVP", &mvp_);
}

Example_Matrices::~Example_Matrices() {
	glDeleteBuffers(1, &vbo_);
	glDeleteVertexArrays(1, &vao_);
}

void Example_Matrices::Update(float deltaTime) {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);
}

void Example_Matrices::GetEnvRequirement(AppEnv& env) {
	Example::GetEnvRequirement(env);
	env.depthTest = false;
}

Example_ColoredCube::Example_ColoredCube() {
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glm::mat4 proj = glm::perspective(45.f, defaultAspect, 0.1f, 100.f);
	glm::mat4 view = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 model = glm::mat4(1.f);
	glm::mat4 mvp = proj * view * model;

	GLfloat vertexBufferData[] = {
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	GLfloat colorBufferData[] = {
		0.583f, 0.771f, 0.014f,
		0.609f, 0.115f, 0.436f,
		0.327f, 0.483f, 0.844f,
		0.822f, 0.569f, 0.201f,
		0.435f, 0.602f, 0.223f,
		0.310f, 0.747f, 0.185f,
		0.597f, 0.770f, 0.761f,
		0.559f, 0.436f, 0.730f,
		0.359f, 0.583f, 0.152f,
		0.483f, 0.596f, 0.789f,
		0.559f, 0.861f, 0.639f,
		0.195f, 0.548f, 0.859f,
		0.014f, 0.184f, 0.576f,
		0.771f, 0.328f, 0.970f,
		0.406f, 0.615f, 0.116f,
		0.676f, 0.977f, 0.133f,
		0.971f, 0.572f, 0.833f,
		0.140f, 0.616f, 0.489f,
		0.997f, 0.513f, 0.064f,
		0.945f, 0.719f, 0.592f,
		0.543f, 0.021f, 0.978f,
		0.279f, 0.317f, 0.505f,
		0.167f, 0.620f, 0.077f,
		0.347f, 0.857f, 0.137f,
		0.055f, 0.953f, 0.042f,
		0.714f, 0.505f, 0.345f,
		0.783f, 0.290f, 0.734f,
		0.722f, 0.645f, 0.174f,
		0.302f, 0.455f, 0.848f,
		0.225f, 0.587f, 0.040f,
		0.517f, 0.713f, 0.338f,
		0.053f, 0.959f, 0.120f,
		0.393f, 0.621f, 0.362f,
		0.673f, 0.211f, 0.457f,
		0.820f, 0.883f, 0.371f,
		0.982f, 0.099f, 0.879f
	};

	glGenBuffers(2, vbo_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorBufferData), colorBufferData, GL_STATIC_DRAW);

	shader_->Load(ShaderTypeVertex, "shaders/colored.vert");
	shader_->Load(ShaderTypeFragment, "shaders/colored.frag");
	shader_->Link();
	shader_->Use();

	shader_->SetUniform("MVP", &mvp);
}

Example_ColoredCube::~Example_ColoredCube() {
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(2, vbo_);
}

void Example_ColoredCube::Update(float deltaTime) {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

Example_TexturedCube::Example_TexturedCube() {
	texture0_ = new Texture;
	texture1_ = new Texture;

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(2, vbo_);

	GLfloat vertexBufferData[] = { 
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

	GLfloat uvBufferData[] = { 
		0.000059f, 1.0f - 0.000004f,
		0.000103f, 1.0f - 0.336048f,
		0.335973f, 1.0f - 0.335903f,
		1.000023f, 1.0f - 0.000013f,
		0.667979f, 1.0f - 0.335851f,
		0.999958f, 1.0f - 0.336064f,
		0.667979f, 1.0f - 0.335851f,
		0.336024f, 1.0f - 0.671877f,
		0.667969f, 1.0f - 0.671889f,
		1.000023f, 1.0f - 0.000013f,
		0.668104f, 1.0f - 0.000013f,
		0.667979f, 1.0f - 0.335851f,
		0.000059f, 1.0f - 0.000004f,
		0.335973f, 1.0f - 0.335903f,
		0.336098f, 1.0f - 0.000071f,
		0.667979f, 1.0f - 0.335851f,
		0.335973f, 1.0f - 0.335903f,
		0.336024f, 1.0f - 0.671877f,
		1.000004f, 1.0f - 0.671847f,
		0.999958f, 1.0f - 0.336064f,
		0.667979f, 1.0f - 0.335851f,
		0.668104f, 1.0f - 0.000013f,
		0.335973f, 1.0f - 0.335903f,
		0.667979f, 1.0f - 0.335851f,
		0.335973f, 1.0f - 0.335903f,
		0.668104f, 1.0f - 0.000013f,
		0.336098f, 1.0f - 0.000071f,
		0.000103f, 1.0f - 0.336048f,
		0.000004f, 1.0f - 0.671870f,
		0.336024f, 1.0f - 0.671877f,
		0.000103f, 1.0f - 0.336048f,
		0.336024f, 1.0f - 0.671877f,
		0.335973f, 1.0f - 0.335903f,
		0.667969f, 1.0f - 0.671889f,
		1.000004f, 1.0f - 0.671847f,
		0.667979f, 1.0f - 0.335851f 
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufferData), uvBufferData, GL_STATIC_DRAW);

	shader_->Load(ShaderTypeVertex, "shaders/texture.vert");
	shader_->Load(ShaderTypeFragment, "shaders/texture.frag");
	shader_->Link();

	glm::mat4 proj = glm::perspective(45.f, defaultAspect, 0.1f, 100.f);
	glm::mat4 view = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 model = glm::mat4(1.f);
	mvp_ = proj * view * model;
	shader_->SetBlockUniform("Matrices", "MVP", &mvp_);

	texture0_->Load("textures/uvtemplate.dds");
	texture1_->Load("textures/uvtemplate.bmp");

	glActiveTexture(GL_TEXTURE0);
	texture0_->Use();
	shader_->SetUniform("textureSamplerDDS", 0);

	glActiveTexture(GL_TEXTURE1);
	texture1_->Use();
	shader_->SetUniform("textureSamplerBmp", 1);
}

Example_TexturedCube::~Example_TexturedCube() {
	delete texture0_;
	delete texture1_;

	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(2, vbo_);
}

void Example_TexturedCube::Update(float deltaTime) {
	shader_->Use();

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

Example_KeyboardAndMouse::Example_KeyboardAndMouse() {
	coordShader_ = new Shader;
	coordShader_->Load(ShaderTypeVertex, "shaders/coord.vert");
	coordShader_->Load(ShaderTypeFragment, "shaders/coord.frag");
	coordShader_->Link();

	Input::SetEnabled(true);
	glGenVertexArrays(1, &vao_);

	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
}

Example_KeyboardAndMouse::~Example_KeyboardAndMouse() {
	delete coordShader_;

	Input::SetEnabled(false);
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(1, &vbo_);
}

void Example_KeyboardAndMouse::Update(float deltaTime) {
	coordShader_->Use();

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glm::mat4 view = Input::GetViewMatrix();
	glm::vec4 x = view[0], y = view[1], z = view[2];

	float coord[] = { 0, 0, 0, x.x, x.y, x.z,  0, 0, 0, y.x, y.y, y.z , 0, 0, 0, z.x, z.y, z.z };
	float color[] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

	for (GLint i = 0; i < 3; ++i) {
		coordShader_->SetUniform("color", color + i * 3);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * sizeof(float), coord + i * 6);
		glDrawArrays(GL_LINES, 0, 18);
	}

	glDisableVertexAttribArray(0);

	glm::mat4 mvp = Input::GetProjectionMatrix() * Input::GetViewMatrix();
	shader_->SetBlockUniform("Matrices", "MVP", &mvp);
	Example_TexturedCube::Update(deltaTime);
}