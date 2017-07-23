#pragma once
#include "loader.h"
#include "example.h"
#include "texture.h"

class Example_LightMaps : public Example {
public:
	Example_LightMaps() {
		shader_->Load(ShaderTypeVertex, "shaders/lightmaps.vert");
		shader_->Load(ShaderTypeFragment, "shaders/lightmaps.frag");
		shader_->Link();
		shader_->Bind();

		camera_->Reset(glm::vec3(-20, 5, -15), glm::vec3(0));
		const glm::mat4& view = camera_->GetViewMatrix();
		const glm::mat4& proj = camera_->GetProjMatrix();
		glm::mat4 MVP = proj * view * glm::mat4(1);
		shader_->SetUniform("MVP", &MVP);

		texture_ = new Texture2D();
		texture_->Load("textures/lightmap.dds");

		texture_->Bind(GL_TEXTURE0);
		shader_->SetUniform("textureSampler", 0);

		modelInfo_ = new ModelInfo;
		ModelLoader::Load("models/room.obj", *modelInfo_);

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		glGenBuffers(COUNT_OF(vbo_), vbo_);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glBufferData(GL_ARRAY_BUFFER, modelInfo_->vertices.size() * sizeof(glm::vec3), &modelInfo_->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
		glBufferData(GL_ARRAY_BUFFER, modelInfo_->uvs.size() * sizeof(glm::vec2), &modelInfo_->uvs[0], GL_STATIC_DRAW);
	}

	~Example_LightMaps() {
		delete texture_;
		delete modelInfo_;

		glDeleteVertexArrays(1, &vao_);
		glDeleteBuffers(COUNT_OF(vbo_), vbo_);
	}

public:
	virtual void Update(float deltaTime) {
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, modelInfo_->vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	virtual void GetEnvRequirement(AppEnv& env) {
		Example::GetEnvRequirement(env);
		env.cullFace = true;
	}

private:
	GLuint vao_;
	GLuint vbo_[2];

	Texture2D* texture_;
	ModelInfo* modelInfo_;
};
