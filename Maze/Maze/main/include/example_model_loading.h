#pragma once
#include "example.h"

class Example_ModelLoading : public Example {
public:
	Example_ModelLoading() {
		mesh_ = new Mesh;
		texture_ = new Texture;
		info_ = new ModelInfo;

		texture_->Load("textures/suzanne_uvmap.dds");

		shader_->Load(ShaderTypeVertex, "shaders/model_loading.vert");
		shader_->Load(ShaderTypeFragment, "shaders/model_loading.frag");
		shader_->Link();
		shader_->Use();

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		mesh_->Load("models/suzanne.obj");
		//ModelLoader::Load("models/suzanne.obj", *info_);

		/*
		glGenBuffers(COUNT_OF(vbo_), vbo_);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glBufferData(GL_ARRAY_BUFFER, info_->vertices.size() * sizeof(glm::vec3), &info_->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
		glBufferData(GL_ARRAY_BUFFER, info_->uvs.size() * sizeof(glm::vec2), &info_->uvs[0], GL_STATIC_DRAW);
		*/

		//glActiveTexture(GL_TEXTURE0);
		//texture_->Use();
		shader_->SetUniform("sampler", 0);

		camera_->Reset(glm::vec3(0, 2, 3), glm::vec3(0));
	}

	~Example_ModelLoading() {
		delete texture_;
		delete info_;
		delete mesh_;

		glDeleteVertexArrays(1, &vao_);
		glDeleteBuffers(COUNT_OF(vbo_), vbo_);
	}

public:
	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);
		
		glm::mat4 mvp = camera_->GetProjMatrix() * camera_->GetViewMatrix();
		shader_->SetUniform("MVP", &mvp);

		mesh_->Render();
		/*
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, info_->vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		*/
	}

private:
	Mesh* mesh_;
	ModelInfo* info_;
	Texture* texture_;
	GLuint vao_, vbo_[2];
};
