#pragma once
#include "example.h"

class Example_BasicTessellation : public Example {
public:
	Example_BasicTessellation() {
		shader_ = new Shader;
		shader_->Load(ShaderTypeVertex, "shaders/tessellation.vert");
		shader_->Load(ShaderTypeTessellationControl, "shaders/tessellation.tesc");
		shader_->Load(ShaderTypeTessellationEvaluation, "shaders/tessellation.tese");
		shader_->Load(ShaderTypeFragment, "shaders/tessellation.frag");
		shader_->Link();

		displacementMap_ = new Texture;
		displacementMap_->Load("textures/heightmap.jpg");

		colorMap_ = new Texture;
		colorMap_->Load("textures/diffuse.jpg");

		mesh_ = new Mesh;
		mesh_->Load("models/quad2.obj");

		shader_->Link();

		camera_->Reset(glm::vec3(0.f, 1.f, -5.f), glm::vec3(0.f, -0.2f, 1.f));
	}

	~Example_BasicTessellation() {
	}

public:
	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);

		shader_->Use();

		shader_->SetUniform("gEyeWorldPos", &camera_->GetPosition());

		glm::mat4 m = glm::mat4(1);
		glm::mat4 vp = camera_->GetProjMatrix() * camera_->GetViewMatrix();

		shader_->SetUniform("gWorld", &m);
		shader_->SetUniform("gVP", &vp);

		glActiveTexture(GL_TEXTURE0);
		displacementMap_->Use();
		shader_->SetUniform("gDisplacementMap", 0);

		glActiveTexture(GL_TEXTURE1);
		colorMap_->Use();
		shader_->SetUniform("gColorMap", 1);

		shader_->SetUniform("gDispFactor", 0.25f);

		mesh_->Render(GL_PATCHES);
	}

	virtual void GetEnvRequirement(AppEnv& env) {
		Example::GetEnvRequirement(env);
	}

private:
	//ModelInfo* info_;
	Mesh* mesh_;
	Shader* shader_;

	//GLuint vao_, vbo_[3];

	Texture* colorMap_;
	Texture* displacementMap_;
};
