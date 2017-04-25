#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "skybox.h"
#include "shader.h"
#include "loader.h"
#include "texture.h"
#include "utilities.h"

SkyBox::SkyBox(const SkyBoxInitParameter& p) :p_(p) {
	shader_ = new Shader;
	shader_->Load(ShaderTypeVertex, "shaders/skybox.vert");
	shader_->Load(ShaderTypeFragment, "shaders/skybox.frag");
	shader_->Link();

	cubemapTexture_ = new CubemapTexture;
	cubemapTexture_->Load(p.posx, p.negx, p.posy, p.negy, p.posz, p.negz);

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(COUNT_OF(vbo_), vbo_);

	modelInfo_ = new ModelInfo;
	ModelLoader::Load("models/sphere.obj", *modelInfo_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glBufferData(GL_ARRAY_BUFFER, modelInfo_->vertices.size() * sizeof(glm::vec3), &modelInfo_->vertices[0], GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	//glBufferData(GL_ARRAY_BUFFER, modelInfo_->normals.size() * sizeof(glm::vec3), &modelInfo_->normals[0], GL_STATIC_DRAW);
}

SkyBox::~SkyBox() {
	delete shader_;
	delete modelInfo_;
	delete cubemapTexture_;

	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(COUNT_OF(vbo_), vbo_);
}

void SkyBox::Render() {
	shader_->Use();

	GLint oldCullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);

	GLint oldDepthFuncMode;
	glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	glm::mat4 m(1);
	const float scale = 0.2f;
	m = glm::translate(m, p_.camera->GetPosition());
	m = glm::scale(m, glm::vec3(scale, scale, scale));
	
	glm::mat4 mvp = p_.camera->GetProjMatrix() * p_.camera->GetViewMatrix() * m;
	shader_->SetUniform("MVP", &mvp);

	glActiveTexture(GL_TEXTURE0);
	cubemapTexture_->Use();
	shader_->SetUniform("textureSampler", 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//glEnableVertexAttribArray(1);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glDrawArrays(GL_TRIANGLES, 0, modelInfo_->vertices.size());

	glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);

	glCullFace(oldCullFaceMode);
	glDepthFunc(oldDepthFuncMode);
}
