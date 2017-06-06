#pragma once
#include "text2d.h"
#include "loader.h"
#include "example.h"
#include "texture.h"

class Example_VBOIndexing : public Example {
public:
	Example_VBOIndexing() {
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

	~Example_VBOIndexing() {
		delete info_;
		delete text_;
		delete texture_;

		glDeleteVertexArrays(1, &vao_);
		glDeleteBuffers(COUNT_OF(vbo_), vbo_);
	}

public:
	virtual void Update(float deltaTime) {
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

	virtual void GetEnvRequirement(AppEnv& env) {
		Example::GetEnvRequirement(env);
		//env.blend = true;
		env.cullFace = true;
	}

private:

	Text2D* text_;

	ModelInfo* info_;
	Texture* texture_;
	GLuint vao_, vbo_[4];
	std::vector<unsigned> indices_;
};
