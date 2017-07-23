#pragma once
#include "example.h"
#include "texture.h"

class Example_Billboards2 : public Example {
public:
	Example_Billboards2() {
		texture_ = new Texture2D;
		texture_->Load("textures/monster.png");

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		glGenBuffers(COUNT_OF(vbo_), vbo_);

		shader_->Load(ShaderTypeVertex, "shaders/billboard2.vert");
		shader_->Load(ShaderTypeGeometry, "shaders/billboard2.geom");
		shader_->Load(ShaderTypeFragment, "shaders/billboard2.frag");
		shader_->Link();

		glm::vec3 positions[rowNumber * colNumber];
		for (int i = 0; i < rowNumber; ++i) {
			for (int j = 0; j < colNumber; ++j) {
				positions[i * colNumber + j] = glm::vec3(j, 0, i);
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_STATIC_DRAW);
	}

	~Example_Billboards2() {
		delete texture_;

		glDeleteVertexArrays(1, &vao_);
		glDeleteBuffers(COUNT_OF(vbo_), vbo_);
	}

public:
	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);

		shader_->Bind();

		texture_->Bind(GL_TEXTURE0);
		shader_->SetUniform("textureSampler", 0);
		shader_->SetUniform("cameraPosition", &camera_->GetPosition());
		glm::mat4 VP = camera_->GetProjMatrix() * camera_->GetViewMatrix();
		shader_->SetUniform("VP", &VP);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glDrawArrays(GL_POINTS, 0, rowNumber * colNumber);
		glDisableVertexAttribArray(0);
	}

private:
	Texture2D* texture_;
	GLuint vao_;
	GLuint vbo_[2];

	static const int rowNumber = 10;
	static const int colNumber = 10;
};
