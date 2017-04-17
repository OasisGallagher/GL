#include "mathf.h"
#include "input.h"
#include "shader.h"
#include "loader.h"
#include "text2d.h"
#include "camera.h"
#include "texture.h"
#include "example.h"
#include "utilities.h"

extern App app;
static const float defaultAspect = 4.f / 3.f;
static const float moveSpeed = 3.f;

Example::Example() {
	shader_ = new Shader;
	camera_ = new Camera;
	input_ = new GLFWInput(app.GetWindow());
}

Example::~Example() {
	delete shader_;
	delete camera_;
	delete input_;
}

void Example::GetEnvRequirement(AppEnv& env) {
	env.blend = false;
	env.cullFace = false;
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

	camera_->Reset(glm::vec3(4, 3, 3), glm::vec3(0), glm::vec3(0, 1, 0));

	glm::mat4 model = glm::mat4(1.f);
	mvp_ = camera_->GetProjMatrix() * camera_->GetViewMatrix() * model;
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

	camera_->Reset(glm::vec3(0, 0, 5), glm::vec3(0), glm::vec3(0, 1, 0));

	glGenVertexArrays(1, &vao_);

	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
}

Example_KeyboardAndMouse::~Example_KeyboardAndMouse() {
	delete coordShader_;

	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(1, &vbo_);
}

void Example_KeyboardAndMouse::Update(float deltaTime) {
	coordShader_->Use();

	if (input_->IsKeyDown(KeyCodeForward)) {
		camera_->Walk(-deltaTime * moveSpeed);
	}

	if (input_->IsKeyDown(KeyCodeBackward)) {
		camera_->Walk(deltaTime * moveSpeed);
	}

	if (input_->IsKeyDown(KeyCodeLeft)) {
		camera_->Strafe(-deltaTime * moveSpeed);
	}

	if (input_->IsKeyDown(KeyCodeRight)) {
		camera_->Strafe(deltaTime * moveSpeed);
	}

	if (input_->IsKeyDown(KeyCodePitchClockwise)) {
		camera_->Pitch(deltaTime * Mathf::PI / 50);
	}

	if (input_->IsKeyDown(KeyCodePitchAnticlockwise)) {
		camera_->Pitch(-deltaTime * Mathf::PI / 50);
	}

	if (input_->IsKeyDown(KeyCodeYawClockwise)) {
		camera_->Yaw(deltaTime * Mathf::PI / 50);
	}

	if (input_->IsKeyDown(KeyCodeYawAnticlockwise)) {
		camera_->Yaw(-deltaTime * Mathf::PI / 50);
	}

	if (input_->IsKeyDown(KeyCodeRollClockwise)) {
		camera_->Roll(deltaTime * Mathf::PI / 50);
	}

	if (input_->IsKeyDown(KeyCodeRollAnticlockwise)) {
		camera_->Roll(-deltaTime * Mathf::PI / 50);
	}

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	const glm::mat4& view = camera_->GetViewMatrix();

	float coord[] = { 
		0, 0, 0, view[0][0], view[1][0], view[2][0],
		0, 0, 0, view[0][1], view[1][1], view[2][1],
		0, 0, 0, view[0][2], view[1][2], view[2][2]
	};

	float color[] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

	for (GLint i = 0; i < 3; ++i) {
		coordShader_->SetUniform("color", color + i * 3);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * sizeof(float), coord + i * 6);
		glDrawArrays(GL_LINES, 0, 6);
	}
	
	glDisableVertexAttribArray(0);

	glm::mat4 mvp = camera_->GetProjMatrix() * view;
	shader_->SetBlockUniform("Matrices", "MVP", &mvp);
	Example_TexturedCube::Update(deltaTime);
}

Example_ModelLoading::Example_ModelLoading() {
	texture_ = new Texture;
	info_ = new ModelInfo;

	texture_->Load("textures/suzanne_uvmap.dds");

	shader_->Load(ShaderTypeVertex, "shaders/model_loading.vert");
	shader_->Load(ShaderTypeFragment, "shaders/model_loading.frag");
	shader_->Link();
	shader_->Use();

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	ModelLoader::Load("models/suzanne.obj", *info_);

	glGenBuffers(COUNT_OF(vbo_), vbo_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glBufferData(GL_ARRAY_BUFFER, info_->vertices.size() * sizeof(glm::vec3), &info_->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	glBufferData(GL_ARRAY_BUFFER, info_->uvs.size() * sizeof(glm::vec2), &info_->uvs[0], GL_STATIC_DRAW);

	glActiveTexture(GL_TEXTURE0);
	texture_->Use();
	shader_->SetUniform("sampler", 0);

	camera_->Reset(glm::vec3(0, 0, 12), glm::vec3(0), glm::vec3(0, 1, 0));
	glm::mat4 mvp = camera_->GetProjMatrix() * camera_->GetViewMatrix();
	shader_->SetUniform("MVP", &mvp);
}

Example_ModelLoading::~Example_ModelLoading() {
	delete texture_;
	delete info_;

	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(COUNT_OF(vbo_), vbo_);
}

void Example_ModelLoading::Update(float deltaTime) {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glDrawArrays(GL_TRIANGLES, 0, info_->vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

Example_BasicShading::Example_BasicShading() {
	texture_ = new Texture;
	info_ = new ModelInfo;

	texture_->Load("textures/suzanne_uvmap.dds");

	shader_->Load(ShaderTypeVertex, "shaders/basic.vert");
	shader_->Load(ShaderTypeFragment, "shaders/spotlight.frag");
	shader_->Link();
	shader_->Use();

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	ModelLoader::Load("models/suzanne.obj", *info_);

	glGenBuffers(COUNT_OF(vbo_), vbo_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glBufferData(GL_ARRAY_BUFFER, info_->vertices.size() * sizeof(glm::vec3), &info_->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	glBufferData(GL_ARRAY_BUFFER, info_->uvs.size() * sizeof(glm::vec2), &info_->uvs[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[2]);
	glBufferData(GL_ARRAY_BUFFER, info_->normals.size() * sizeof(glm::vec3), &info_->normals[0], GL_STATIC_DRAW);

	glActiveTexture(GL_TEXTURE0);
	texture_->Use();
	shader_->SetUniform("textureSampler", 0);

	camera_->Reset(glm::vec3(6, 0, 6), glm::vec3(0), glm::vec3(0, 1, 0));
	glm::mat4 m = glm::mat4(1.f);
	const glm::mat4& view = camera_->GetViewMatrix();
	const glm::mat4& proj = camera_->GetProjMatrix();

	glm::mat4 mvp = proj * view * m;

	shader_->SetUniform("M", &m);
	shader_->SetUniform("V", &view);
	shader_->SetUniform("MVP", &mvp);

	glm::vec3 lightPos(4, 0, 4);
	shader_->SetUniform("LightPosition_worldspace", &lightPos);
}

Example_BasicShading::~Example_BasicShading() {
	delete info_;
	delete texture_;

	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(COUNT_OF(vbo_), vbo_);
}

void Example_BasicShading::GetEnvRequirement(AppEnv& env) {
	Example::GetEnvRequirement(env);
	env.cullFace = true;
}

void Example_BasicShading::Update(float deltaTime) {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glDrawArrays(GL_TRIANGLES, 0, info_->vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

Example_VBOIndexing::Example_VBOIndexing() {
	texture_ = new Texture;
	info_ = new ModelInfo;
	text_ = new Text2D;

	text_->Load("textures/font.dds");
	/*
	texture_->Load("textures/suzanne_uvmap.dds");

	shader_->Load(ShaderTypeVertex, "shaders/basic.vert");
	shader_->Load(ShaderTypeFragment, "shaders/basic.frag");
	shader_->Link();
	shader_->Use();

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	ModelLoader::Load("models/suzanne.obj", *info_);
	VBOIndexer::Index(*info_, indices_, *info_);

	glGenBuffers(COUNT_OF(vbo_), vbo_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glBufferData(GL_ARRAY_BUFFER, info_->vertices.size() * sizeof(glm::vec3), &info_->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	glBufferData(GL_ARRAY_BUFFER, info_->uvs.size() * sizeof(glm::vec2), &info_->uvs[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[2]);
	glBufferData(GL_ARRAY_BUFFER, info_->normals.size() * sizeof(glm::vec3), &info_->normals[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned), &indices_[0], GL_STATIC_DRAW);

	glActiveTexture(GL_TEXTURE0);
	texture_->Use();
	shader_->SetUniform("textureSampler", 0);

	camera_->Reset(glm::vec3(6, 0, 6), glm::vec3(0), glm::vec3(0, 1, 0));
	glm::mat4 m = glm::mat4(1.f);
	const glm::mat4& view = camera_->GetViewMatrix();
	const glm::mat4& proj = camera_->GetProjMatrix();

	glm::mat4 mvp = proj * view * m;

	shader_->SetUniform("M", &m);
	shader_->SetUniform("V", &view);
	shader_->SetUniform("MVP", &mvp);

	glm::vec3 lightPos(4, 0, 4);
	shader_->SetUniform("LightPosition_worldspace", &lightPos);*/
}

Example_VBOIndexing::~Example_VBOIndexing() {
	delete info_;
	delete text_;
	delete texture_;

	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(COUNT_OF(vbo_), vbo_);
}

void Example_VBOIndexing::GetEnvRequirement(AppEnv& env) {
	Example::GetEnvRequirement(env);
	//env.blend = true;
	env.cullFace = true;
}

void Example_VBOIndexing::Update(float deltaTime) {
	/*
	shader_->Use();

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_[3]);
	//glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	*/
	text_->Print("1", 0, 200, 512);
}

Example_NormalMapping::Example_NormalMapping() {
	modelInfo_ = new ModelInfo;
	ModelLoader::Load("models/cylinder.obj", *modelInfo_);

	VBOIndexer::Index(*modelInfo_, indices_, *modelInfo_);

	normal_ = new Texture;
	normal_->Load("textures/normal.bmp");

	diffuse_ = new Texture;
	diffuse_->Load("textures/diffuse.dds");

	specular_ = new Texture;
	specular_->Load("textures/specular.dds");

	shader_->Load(ShaderTypeVertex, "shaders/normal_mapping.vert");
	shader_->Load(ShaderTypeFragment, "shaders/normal_mapping.frag");
	shader_->Link();

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(1, &vertexVbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVbo_);
	glBufferData(GL_ARRAY_BUFFER, modelInfo_->vertices.size() * sizeof(glm::vec3), &modelInfo_->vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvVbo_);
	glBindBuffer(GL_ARRAY_BUFFER, uvVbo_);
	glBufferData(GL_ARRAY_BUFFER, modelInfo_->uvs.size() * sizeof(glm::vec2), &modelInfo_->uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalVbo_);
	glBindBuffer(GL_ARRAY_BUFFER, normalVbo_);
	glBufferData(GL_ARRAY_BUFFER, modelInfo_->normals.size() * sizeof(glm::vec3), &modelInfo_->normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &indexVbo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned), &indices_[0], GL_STATIC_DRAW);

	camera_->Reset(glm::vec3(3, 8, 5), glm::vec3(0), glm::vec3(0, 1, 0));
	const glm::mat4& proj = camera_->GetProjMatrix();
	const glm::mat4& view = camera_->GetViewMatrix();

	glm::mat4 mvp = proj * view;
	
	shader_->SetUniform("MVP", &mvp);
	shader_->SetUniform("V", &view);

	glActiveTexture(GL_TEXTURE0);
	diffuse_->Use();
	shader_->SetUniform("diffuse", 0);
	
	glActiveTexture(GL_TEXTURE1);
	specular_->Use();
	shader_->SetUniform("specular", 1);

	glm::vec3 lightPosition(3, 8, 5);
	shader_->SetUniform("lightPosition_worldspace", &lightPosition);
}

Example_NormalMapping::~Example_NormalMapping() {
	delete modelInfo_;
	delete normal_;
	delete diffuse_;
	delete specular_;

	glDeleteVertexArrays(1, &vao_);
	glDeleteVertexArrays(1, &uvVbo_);
	glDeleteBuffers(1, &vertexVbo_);
	glDeleteBuffers(1, &indexVbo_);
	glDeleteBuffers(1, &normalVbo_);
}

void Example_NormalMapping::Update(float deltaTime) {
	shader_->Use();

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVbo_);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvVbo_);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalVbo_);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}
