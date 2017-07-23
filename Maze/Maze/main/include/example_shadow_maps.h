#pragma once
#include "loader.h"
#include "example.h"
#include "render_state.h"
#include "render_target.h"

class Example_ShadowMaps : public Example {
public:
	Example_ShadowMaps() {
		texture_ = new Texture2D;
		texture_->Load("textures/room_uvmap.dds");

		quad_.Load("models/quad_r.obj");
		room_.Load("models/room_thickwalls.obj");

		depthShader_ = new Shader;
		depthShader_->Load(ShaderTypeVertex, "shaders/depth.vert");
		depthShader_->Load(ShaderTypeFragment, "shaders/depth.frag");
		depthShader_->Link();

		shadowShader_ = new Shader;
		shadowShader_->Load(ShaderTypeVertex, "shaders/shadow.vert");
		shadowShader_->Load(ShaderTypeFragment, "shaders/shadow.frag");
		shadowShader_->Link();

		renderTarget_.Create(Globals::kWindowWidth, Globals::kWindowHeight);
		renderTarget_.AddDepthTexture(GL_DEPTH_COMPONENT16, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		shader_->Load(ShaderTypeVertex, "shaders/basic_shadow.vert");
		shader_->Load(ShaderTypeFragment, "shaders/basic_shadow.frag");
		shader_->Link();

		camera_->Reset(glm::vec3(40, 20, -10), glm::vec3(0));

		glm::vec3 LightInvDirection_worldspace(0.5f, 2, 2);
		shader_->SetUniform("LightInvDirection_worldspace", &LightInvDirection_worldspace);
	}

	~Example_ShadowMaps() {
		delete texture_;

		delete depthShader_;
		delete shadowShader_;
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

		glViewport(0, 0, Globals::kWindowWidth, Globals::kWindowHeight);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glm::mat4 m(1);
		shader_->SetUniform("M", &m);
		shader_->SetUniform("V", &camera_->GetViewMatrix());

		m = camera_->GetProjMatrix() * camera_->GetViewMatrix() * m;
		shader_->SetUniform("MVP", &m);

		shader_->Bind();
		shader_->SetUniform("ShadowMVP", &shadowMVP_);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderTarget_.GetDepthTexture());
		shader_->SetUniform("shadowSampler", 0);

		texture_->Bind(GL_TEXTURE1);
		shader_->SetUniform("textureSampler", 1);

		room_.Render();
	}

	void ShadowMapPass() {
		renderTarget_.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		renderTarget_.Bind(nullptr, 0);

		RenderState::PushCullFaceEnabled(GL_TRUE);
		RenderState::PushCullFaceFunc(GL_BACK);

		depthShader_->Bind();

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

		RenderState::PopCullFaceEnabled();
		RenderState::PopCullFaceFunc();

		room_.Render();
	}

	void RenderShadowMap() {
		glViewport(0, 0, 128, 96);

		shadowShader_->Bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderTarget_.GetDepthTexture());
		shadowShader_->SetUniform("sampler", 0);

		quad_.Render();
	}

private:
	glm::mat4 shadowMVP_;

	Texture2D* texture_;

	Mesh room_;
	Mesh quad_;

	Shader* depthShader_;
	Shader* shadowShader_;

	RenderTarget renderTarget_;
};
