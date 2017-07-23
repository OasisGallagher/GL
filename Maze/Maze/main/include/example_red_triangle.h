#pragma once
#include "example.h"

class Example_RedTriangle : public Example {
public:
	Example_RedTriangle() {
		const GLfloat data[] = {
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
		};

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);
		glGenBuffers(1, &vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
		shader_->Load(ShaderTypeVertex, "shaders/simple.vert");
		shader_->Load(ShaderTypeFragment, "shaders/simple.frag");
		shader_->Link();
	}

	~Example_RedTriangle() {
		glDeleteBuffers(1, &vbo_);
		glDeleteVertexArrays(1, &vao_);
	}

public:
	virtual void Update(float deltaTime) {
		shader_->Bind();

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(0);
	}

	virtual void GetEnvRequirement(AppEnv& env) {
		Example::GetEnvRequirement(env);
		env.depthTest = false;
	}

private:
	GLuint vao_, vbo_;
};
