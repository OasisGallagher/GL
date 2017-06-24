#pragma once
#include "example.h"
#include "texture.h"

class Example_Billboards : public Example {
public:
	Example_Billboards() {
		texture_ = new Texture;
		texture_->Load("textures/billboard.dds");

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		glGenBuffers(COUNT_OF(vbo_), vbo_);

		const GLfloat vertexBufferData[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
		};

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_DYNAMIC_DRAW);

		shader_->Load(ShaderTypeVertex, "shaders/billboard.vert");
		shader_->Load(ShaderTypeFragment, "shaders/billboard.frag");
		shader_->Link();

		camera_->Reset(glm::vec3(0, 0, 4), glm::vec3(0));
	}

	~Example_Billboards() {
		glDeleteVertexArrays(1, &vao_);
		glDeleteBuffers(COUNT_OF(vbo_), vbo_);
	}

public:
	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);

		shader_->Use();

		glActiveTexture(GL_TEXTURE0);
		texture_->Use();
		shader_->SetUniform("textureSampler", 0);

		const glm::mat4& view = camera_->GetViewMatrix();
		glm::vec3 cameraRight_worldspace(view[0][0], view[1][0], view[2][0]);
		glm::vec3 cameraUp_worldspace(view[0][1], view[1][1], view[2][1]);

		shader_->SetUniform("cameraRight_worldspace", &cameraRight_worldspace);
		shader_->SetUniform("cameraUp_worldspace", &cameraUp_worldspace);

		glm::vec3 billboardPosition(0, 0.5f, 0);
		shader_->SetUniform("billboardPosition", &billboardPosition);

		glm::vec2 billboardSize(1.f, 0.125f);
		shader_->SetUniform("billboardSize", &billboardSize);

		glm::mat4 VP = camera_->GetProjMatrix() * camera_->GetViewMatrix();
		shader_->SetUniform("VP", &VP);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(0);
	}

	virtual void GetEnvRequirement(AppEnv& env) {
		Example::GetEnvRequirement(env);
	}

private:
	Texture* texture_;

	GLuint vao_, vbo_[1];
};
