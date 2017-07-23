#include "example.h"

class Example_Matrices : public Example {
public:
	Example_Matrices() {
		glm::mat4 proj = glm::perspective(45.f, (float)Globals::kWindowWidth / Globals::kWindowHeight, 0.1f, 100.f);
		glm::mat4 view = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glm::mat4 model = glm::mat4(1.f);
		mvp_ = proj * view * model;

		GLfloat vertexBufferData[] = {
			-1.f, -1.f, 0.f,
			1.f, -1.f, 0.f,
			0.f, 1.f, 0.f
		};

		GLushort elementBufferData[] = {
			0, 1, 2
		};

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		glGenBuffers(1, &vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

		shader_->Load(ShaderTypeVertex, "shaders/simple_transform.vert");
		shader_->Load(ShaderTypeFragment, "shaders/simple_transform.frag");
		shader_->Link();

		shader_->Bind();
		shader_->SetUniform("MVP", &mvp_);
	}

	~Example_Matrices() {
		glDeleteBuffers(1, &vbo_);
		glDeleteVertexArrays(1, &vao_);
	}

public:
	virtual void Update(float deltaTime) {
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
	glm::mat4 mvp_;
};
