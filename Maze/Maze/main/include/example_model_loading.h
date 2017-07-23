#pragma once
#include "example.h"
#include "render_target.h"

class Example_ModelLoading : public Example {
public:
	Example_ModelLoading() {
		mesh_ = new Mesh;
		info_ = new ModelInfo;
		texture_ = new Texture2D;

		renderTarget_.Create(Globals::kWindowWidth, Globals::kWindowHeight);
		blurRenderTarget_[0].Create(Globals::kWindowWidth, Globals::kWindowHeight);
		blurRenderTarget_[1].Create(Globals::kWindowWidth, Globals::kWindowHeight);

		quad_.Load("models/quad_r.obj");

		hdrShader_.Load(ShaderTypeVertex, "shaders/hdr.vert");
		hdrShader_.Load(ShaderTypeFragment, "shaders/hdr.frag");
		hdrShader_.Link();
		hdrShader_.SetUniform("tex", Globals::HDRTextureIndex);
		hdrShader_.SetUniform("blur", Globals::ColorTextureIndex);

		texture_->Load("textures/suzanne_uvmap.dds");

		//shader_->Load(ShaderTypeVertex, "shaders/model_loading.vert");
		//shader_->Load(ShaderTypeFragment, "shaders/model_loading.frag");
		shader_->Load("shaders/model_loading.shader");
		shader_->Link();
		shader_->SetUniform("diffuseTex", Globals::ColorTextureIndex);

		blurShader_.Load(ShaderTypeVertex, "shaders/blur.vert");
		blurShader_.Load(ShaderTypeFragment, "shaders/blur.frag");
		blurShader_.Link();
		blurShader_.SetUniform("source", Globals::ColorTextureIndex);

		// Scene.
		renderTarget_.AddRenderTexture(GL_RGB32F, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		// Bloom.
		renderTarget_.AddRenderTexture(GL_RGB32F, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		renderTarget_.AddDepthRenderBuffer();

		for (int i = 0; i < 2; ++i) {
			blurRenderTarget_[i].AddRenderTexture(GL_RGB32F, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		//	blurRenderTarget_[i].AddDepthRenderBuffer();
		}
		
		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		mesh_->Load("models/suzanne.obj");
		
		DirectionalLight light;
		light.color = glm::vec3(0.7f);
		light.ambientIntensity = 0.3f;
		light.diffuseIntensity = 0.5f;
		light.direction = glm::vec3(0) - glm::vec3(0, 7, 5);
		shader_->SetBlock("DirectionalLight", &light);

		camera_->Reset(glm::vec3(0, 1, 5), glm::vec3(0));
		hdr_ = false;
		exposure_ = 1.f;
	}

	~Example_ModelLoading() {
		delete texture_;
		delete info_;
		delete mesh_;

		glDeleteVertexArrays(1, &vao_);
		glDeleteBuffers(COUNT_OF(vbo_), vbo_);
	}

public:
	virtual void GetEnvRequirement(AppEnv& env) {
		Example::GetEnvRequirement(env);
		env.backgroundColor = glm::vec4(0);
	}

	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);

		UpdateCommand();

		renderTarget_.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderTarget_.Bind(NULL, 0);

		glm::mat4 mvp = camera_->GetProjMatrix() * camera_->GetViewMatrix();
		glm::mat4 identity;
		shader_->SetUniform("M", &identity);
		shader_->SetUniform("MVP", &mvp);

		texture_->Bind(Globals::ColorTexture);

		shader_->Bind();

		shader_->SetUniform("cameraPosition", &camera_->GetPosition());
		shader_->SetUniform("specularPower", 0.2f);
		shader_->SetUniform("materialSpecularIntensity", 4.3f);

		mesh_->Render();

		// blur.
		blurShader_.Bind();
		int first = 1, horizontal = 1, amount = 10;
		for (int i = 0; i < amount; ++i) {
			blurRenderTarget_[horizontal].Bind(NULL, 0);

			blurShader_.SetUniform("horizontal", horizontal);
			glActiveTexture(Globals::ColorTexture);
			glBindTexture(GL_TEXTURE_2D, (first != 0) ? renderTarget_.GetRenderTexture(1) : blurRenderTarget_[1 - horizontal].GetRenderTexture(0));
			quad_.Render();
			
			blurRenderTarget_[horizontal].Unbind();

			horizontal = 1 - horizontal;
			first = 0;

		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(Globals::ColorTexture);
		glBindTexture(GL_TEXTURE_2D, blurRenderTarget_[1 - horizontal].GetRenderTexture(0));

		glActiveTexture(Globals::HDRTexture);
		glBindTexture(GL_TEXTURE_2D, renderTarget_.GetRenderTexture(0));

		hdrShader_.SetUniform("HDR", hdr_);
		hdrShader_.SetUniform("exposure", exposure_);
		hdrShader_.Bind();

		quad_.Render();
	}

private:
	void UpdateCommand() {
		Input* input = app.GetInput();
		if (input->IsKeyUp(KeyCodeSpace)) {
			hdr_ = !hdr_;
			Debug::Log(std::string("HDR ") + (hdr_ ? "on" : "off"));
		}

		if (input->IsKeyDown(KeyCodeForward)) {
			exposure_ += 0.01f;
			Debug::Log(std::string("exposure = ") + std::to_string(exposure_));
		}

		if (input->IsKeyDown(KeyCodeBackward)) {
			exposure_ -= 0.01f;
			Debug::Log(std::string("exposure = ") + std::to_string(exposure_));
		}
	}

private:
	bool hdr_;
	float exposure_;

	Shader hdrShader_;
	Shader blurShader_;

	Mesh quad_;

	Mesh* mesh_;
	ModelInfo* info_;
	Texture2D* texture_;
	GLuint vao_, vbo_[2];

	RenderTarget renderTarget_;
	RenderTarget blurRenderTarget_[2];
};
