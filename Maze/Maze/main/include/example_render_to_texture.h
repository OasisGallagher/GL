#pragma once
#include "loader.h"
#include "example.h"
#include "render_target.h"

class Example_RenderToTexture : public Example {
public:
	Example_RenderToTexture() {
		elapsed_ = 0.f;

		shader_->Load(ShaderTypeVertex, "shaders/basic.vert");
		shader_->Load(ShaderTypeFragment, "shaders/basic.frag");
		shader_->Link();

		texture_ = new Texture2D;
		texture_->Load("textures/suzanne_uvmap.dds");
		shader_->SetUniform("textureSampler", 0);

		camera_->Reset(glm::vec3(6, 0, 6), glm::vec3(0));

		shader2_ = new Shader;
		shader2_->Load(ShaderTypeVertex, "shaders/render_to_texture.vert");
		shader2_->Load(ShaderTypeFragment, "shaders/render_to_texture.frag");
		shader2_->Link();

		glm::vec3 lightPos(4, 0, 4);
		shader_->SetUniform("LightPosition_worldspace", &lightPos);

		modelInfo_ = new ModelInfo;
		ModelLoader::Load("models/suzanne.obj", *modelInfo_);

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		glGenBuffers(COUNT_OF(vbo_), vbo_);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glBufferData(GL_ARRAY_BUFFER, modelInfo_->vertices.size() * sizeof(glm::vec3), &modelInfo_->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
		glBufferData(GL_ARRAY_BUFFER, modelInfo_->uvs.size() * sizeof(glm::vec2), &modelInfo_->uvs[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[2]);
		glBufferData(GL_ARRAY_BUFFER, modelInfo_->normals.size() * sizeof(glm::vec3), &modelInfo_->normals[0], GL_STATIC_DRAW);

		renderTarget_.Create(Globals::kWindowWidth, Globals::kWindowHeight);
		renderTarget_.AddRenderTexture(GL_RGB32F, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		renderTarget_.AddDepthRenderBuffer();

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Globals::kQuadCoordinates), Globals::kQuadCoordinates, GL_STATIC_DRAW);
	}

	~Example_RenderToTexture() {
		glDeleteVertexArrays(1, &vao_);
		glDeleteVertexArrays(COUNT_OF(vbo_), vbo_);

		delete texture_;
		delete modelInfo_;
	}

public:
	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);
		
		glm::mat4 m = glm::mat4(1.f);
		const glm::mat4& view = camera_->GetViewMatrix();
		const glm::mat4& proj = camera_->GetProjMatrix();

		glm::mat4 mvp = proj * view * m;

		shader_->SetUniform("M", &m);
		shader_->SetUniform("V", &view);
		shader_->SetUniform("MVP", &mvp);

		elapsed_ += deltaTime;

		renderTarget_.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		renderTarget_.Bind(NULL, 0);

		shader_->Bind();
		texture_->Bind(GL_TEXTURE0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[2]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, modelInfo_->vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		
		renderTarget_.Unbind();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		//int width = Globals::kWindowWidth / 2, height = Globals::kWindowHeight / 2;
		//glViewport((Globals::kWindowWidth - width) / 2, (Globals::kWindowHeight - height) / 2, width, height);

		shader2_->Bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderTarget_.GetRenderTexture(0));

		shader2_->SetUniform("textureSampler", 0);
		shader2_->SetUniform("time", elapsed_);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[3]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(0);
	}

private:
	ModelInfo* modelInfo_;
	Texture2D* texture_;

	Shader* shader2_;

	RenderTarget renderTarget_;

	GLuint vao_;
	GLuint vbo_[4];

	float elapsed_;
};
