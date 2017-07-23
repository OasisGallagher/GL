#pragma once
#include "example_textured_cube.h"

class Example_KeyboardAndMouse : public Example_TexturedCube {
public:
	Example_KeyboardAndMouse() {
		coordShader_ = new Shader;
		coordShader_->Load(ShaderTypeVertex, "shaders/coord.vert");
		coordShader_->Load(ShaderTypeFragment, "shaders/coord.frag");
		coordShader_->Link();

		camera_->Reset(glm::vec3(0, 0, 5), glm::vec3(0));

		glGenVertexArrays(1, &vao_);

		glGenBuffers(1, &vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
	}

	~Example_KeyboardAndMouse() {
		delete coordShader_;

		glDeleteVertexArrays(1, &vao_);
		glDeleteBuffers(1, &vbo_);
	}

public:
	virtual void Update(float deltaTime) {
		coordShader_->Bind();

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		const glm::mat4& view = camera_->GetViewMatrix();

		float coord[] = {
			0, 0, 0, view[0][0], view[1][0], view[2][0],
			0, 0, 0, view[0][1], view[1][1], view[2][1],
			0, 0, 0, view[0][2], view[1][2], view[2][2]
		};

		float color[] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

		for (GLint i = 0; i < 3; ++i) {
			coordShader_->SetUniform("color", color + i * 3);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * sizeof(float), coord + i * 6);
			glDrawArrays(GL_LINES, 0, 6);
		}

		glDisableVertexAttribArray(0);

		glm::mat4 mvp = camera_->GetProjMatrix() * view;
		shader_->SetBlockUniform("Matrices", "MVP", &mvp);
		Example_TexturedCube::Update(deltaTime);
	}

private:
	Shader* coordShader_;
	GLuint vao_, vbo_;
};
