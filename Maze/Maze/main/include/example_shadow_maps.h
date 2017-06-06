#pragma once
#include "loader.h"
#include "example.h"

class Example_ShadowMaps : public Example {
public:
	Example_ShadowMaps() {
		texture_ = new Texture;
		texture_->Load("textures/room_uvmap.dds");

		modelInfo_ = new ModelInfo;
		ModelLoader::Load("models/room_thickwalls.obj", *modelInfo_);
		VBOIndexer::Index(*modelInfo_, indices_, *modelInfo_);

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		glGenBuffers(COUNT_OF(vbo_), vbo_);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glBufferData(GL_ARRAY_BUFFER, modelInfo_->vertices.size() * sizeof(glm::vec3), &modelInfo_->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
		glBufferData(GL_ARRAY_BUFFER, modelInfo_->uvs.size() * sizeof(glm::vec2), &modelInfo_->uvs[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[2]);
		glBufferData(GL_ARRAY_BUFFER, modelInfo_->normals.size() * sizeof(glm::vec3), &modelInfo_->normals[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned), &indices_[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[4]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Globals::quadData), Globals::quadData, GL_STATIC_DRAW);

		depthShader_ = new Shader;
		depthShader_->Load(ShaderTypeVertex, "shaders/depth.vert");
		depthShader_->Load(ShaderTypeFragment, "shaders/depth.frag");
		depthShader_->Link();

		shadowShader_ = new Shader;
		shadowShader_->Load(ShaderTypeVertex, "shaders/shadow.vert");
		shadowShader_->Load(ShaderTypeFragment, "shaders/shadow.frag");
		shadowShader_->Link();

		depthTexture_ = new RenderTexture(RenderDepthTexture, WINDOW_WIDTH, WINDOW_HEIGHT);

		shader_->Load(ShaderTypeVertex, "shaders/basic_shadow.vert");
		shader_->Load(ShaderTypeFragment, "shaders/basic_shadow.frag");
		shader_->Link();

		camera_->Reset(glm::vec3(-20, 5, -15/*4, 0, 19*/), glm::vec3(0), glm::vec3(0, 1, 0));

		glm::vec3 LightInvDirection_worldspace(0.5f, 2, 2);
		shader_->SetUniform("LightInvDirection_worldspace", &LightInvDirection_worldspace);
	}

	~Example_ShadowMaps() {
		delete texture_;
		delete modelInfo_;
		delete depthTexture_;

		delete depthShader_;
		delete shadowShader_;

		glDeleteVertexArrays(1, &vao_);
		glDeleteBuffers(COUNT_OF(vbo_), vbo_);
	}

public:
	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);
		ShadowMapPass();
		RenderPass();
		RenderShadowMap();
	}

	virtual void GetEnvRequirement(AppEnv& env) {
		Example::GetEnvRequirement(env);
		env.cullFace = true;
	}

private:
	void RenderPass() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glm::mat4 m(1);
		shader_->SetUniform("M", &m);
		shader_->SetUniform("V", &camera_->GetViewMatrix());

		m = camera_->GetProjMatrix() * camera_->GetViewMatrix() * m;
		shader_->SetUniform("MVP", &m);

		shader_->Use();
		shader_->SetUniform("ShadowMVP", &shadowMVP_);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture_->GetTexture());
		shader_->SetUniform("shadowSampler", 0);

		glActiveTexture(GL_TEXTURE1);
		texture_->Use();
		shader_->SetUniform("textureSampler", 1);

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
		glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}

	void ShadowMapPass() {
		depthTexture_->Use();
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		depthShader_->Use();

		glm::vec3 LightInvDirection_worldspace(0.5f, 2, 2);

		glm::mat4 proj = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
		glm::mat4 view = glm::lookAt(LightInvDirection_worldspace, glm::vec3(0), glm::vec3(0, 1, 0));
		glm::mat4 depthMVP = proj * view * glm::mat4(1);

		glm::mat4 basis(
			glm::vec4(0.5f, 0, 0, 0),
			glm::vec4(0, 0.5f, 0, 0),
			glm::vec4(0, 0, 0.5f, 0),
			glm::vec4(0.5f, 0.5f, 0.5f, 1)
			);

		shadowMVP_ = basis * depthMVP;

		depthShader_->SetUniform("depthMVP", &depthMVP);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_[3]);
		glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);

		glDisableVertexAttribArray(0);
	}

	void RenderShadowMap() {
		glViewport(0, 0, 128, 96);

		shadowShader_->Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture_->GetTexture());
		shadowShader_->SetUniform("sampler", 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[4]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(0);
	}

private:
	GLuint vao_;
	GLuint vbo_[5];
	std::vector<unsigned> indices_;

	glm::mat4 shadowMVP_;

	Texture* texture_;
	ModelInfo* modelInfo_;

	Shader* depthShader_;
	Shader* shadowShader_;

	RenderTexture* depthTexture_;
};
