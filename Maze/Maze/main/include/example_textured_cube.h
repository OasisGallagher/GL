#pragma once
#include "example.h"
#include "texture.h"

class Example_TexturedCube : public Example {
public:
	Example_TexturedCube() {
		texture0_ = new Texture2D;
		texture1_ = new Texture2D;

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		glGenBuffers(2, vbo_);

		GLfloat vertexBufferData[] = {
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f
		};

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

		GLfloat uvBufferData[] = {
			0.000059f, 1.0f - 0.000004f,
			0.000103f, 1.0f - 0.336048f,
			0.335973f, 1.0f - 0.335903f,
			1.000023f, 1.0f - 0.000013f,
			0.667979f, 1.0f - 0.335851f,
			0.999958f, 1.0f - 0.336064f,
			0.667979f, 1.0f - 0.335851f,
			0.336024f, 1.0f - 0.671877f,
			0.667969f, 1.0f - 0.671889f,
			1.000023f, 1.0f - 0.000013f,
			0.668104f, 1.0f - 0.000013f,
			0.667979f, 1.0f - 0.335851f,
			0.000059f, 1.0f - 0.000004f,
			0.335973f, 1.0f - 0.335903f,
			0.336098f, 1.0f - 0.000071f,
			0.667979f, 1.0f - 0.335851f,
			0.335973f, 1.0f - 0.335903f,
			0.336024f, 1.0f - 0.671877f,
			1.000004f, 1.0f - 0.671847f,
			0.999958f, 1.0f - 0.336064f,
			0.667979f, 1.0f - 0.335851f,
			0.668104f, 1.0f - 0.000013f,
			0.335973f, 1.0f - 0.335903f,
			0.667979f, 1.0f - 0.335851f,
			0.335973f, 1.0f - 0.335903f,
			0.668104f, 1.0f - 0.000013f,
			0.336098f, 1.0f - 0.000071f,
			0.000103f, 1.0f - 0.336048f,
			0.000004f, 1.0f - 0.671870f,
			0.336024f, 1.0f - 0.671877f,
			0.000103f, 1.0f - 0.336048f,
			0.336024f, 1.0f - 0.671877f,
			0.335973f, 1.0f - 0.335903f,
			0.667969f, 1.0f - 0.671889f,
			1.000004f, 1.0f - 0.671847f,
			0.667979f, 1.0f - 0.335851f
		};

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufferData), uvBufferData, GL_STATIC_DRAW);

		shader_->Load(ShaderTypeVertex, "shaders/texture.vert");
		shader_->Load(ShaderTypeFragment, "shaders/texture.frag");
		shader_->Link();

		camera_->Reset(glm::vec3(0, 0, 3), glm::vec3(0));

		texture0_->Load("textures/uvtemplate.dds");
		texture1_->Load("textures/uvtemplate.bmp");

		texture0_->Bind(GL_TEXTURE0);
		//shader_->SetUniform("textureSamplerDDS", 0);

		texture1_->Bind(GL_TEXTURE1);
		//shader_->SetUniform("textureSamplerBmp", 1);
	}

	~Example_TexturedCube() {
		delete texture0_;
		delete texture1_;

		glDeleteVertexArrays(1, &vao_);
		glDeleteBuffers(2, vbo_);
	}

public:
	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);

		shader_->Bind();
		
		glm::mat4 model(1);
		model = glm::translate(model, glm::vec3(0, 0, -12));
		mvp_ = camera_->GetProjMatrix() * camera_->GetViewMatrix() * model;
		shader_->SetBlock("Matrices", &mvp_);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

private:
	glm::mat4 mvp_;

	Texture2D* texture0_, *texture1_;
	GLuint vao_, vbo_[2];
};
