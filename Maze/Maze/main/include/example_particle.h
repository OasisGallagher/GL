#pragma once
#include <algorithm>

#include "mesh.h"
#include "texture.h"
#include "example.h"

struct Particle {
	glm::vec3 position, speed;
	GLubyte r, g, b, a;
	float size, angle, weight;
	float life;
	float cameraDistance2;

	bool operator < (const Particle& other) const {
		return cameraDistance2 > other.cameraDistance2;
	}
};

class Example_Particle : public Example {
public:
	Example_Particle() {
		particles_ = new Particle[Globals::maxParticleCount];
		memset(particles_, 0, sizeof(Particle)* Globals::maxParticleCount);

		colorBuffer_ = new GLubyte[Globals::maxParticleCount * 4];
		memset(colorBuffer_, 0, sizeof(GLubyte)* Globals::maxParticleCount * 4);

		positionBuffer_ = new GLfloat[Globals::maxParticleCount * 4];
		memset(positionBuffer_, 0, sizeof(GLfloat)* Globals::maxParticleCount * 4);

		for (int i = 0; i < Globals::maxParticleCount; ++i) {
			particles_[i].life = -1;
			particles_[i].cameraDistance2 = -1;
		}

		camera_->Reset(glm::vec3(0, 0, 20), glm::vec3(0), glm::vec3(0, 1.f, 0));

		texture_ = new Texture;
		texture_->Load("textures/particle.dds");

		shader_->Load(ShaderTypeVertex, "shaders/particle.vert");
		shader_->Load(ShaderTypeFragment, "shaders/particle.frag");
		shader_->Link();

		GLfloat vertexBuffer[] = {
			-0.5f, -0.5f, 0.f,
			0.5f, -0.5f, 0.f,
			-0.5f, 0.5f, 0.f,
			0.5f, 0.5f, 0.f
		};

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		glGenBuffers(COUNT_OF(vbo_), vbo_);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuffer), vertexBuffer, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 4 * Globals::maxParticleCount, nullptr, GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLubyte)* 4 * Globals::maxParticleCount, nullptr, GL_STREAM_DRAW);
	}

	~Example_Particle() {
		delete[] particles_;
		delete[] colorBuffer_;
		delete[] positionBuffer_;

		glDeleteVertexArrays(1, &vao_);
		glDeleteBuffers(COUNT_OF(vbo_), vbo_);
	}

public:
	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);

		CreateNewParticles(deltaTime);
		UpdateParticles(deltaTime);

		SortParticles();
		RenderParticles();
	}

	virtual void GetEnvRequirement(AppEnv& env) {
		Example::GetEnvRequirement(env);
	}

private:
	int FindUnusedParticle() {
		for (int i = lastUsedParticle_; i < Globals::maxParticleCount; ++i) {
			if (particles_[i].life < 0) {
				lastUsedParticle_ = i;
				return i;
			}
		}

		for (int i = 0; i < lastUsedParticle_; ++i) {
			if (particles_[i].life < 0) {
				lastUsedParticle_ = i;
				return i;
			}
		}

		return 0;
	}

	void SortParticles() {
		std::sort(particles_, particles_ + Globals::maxParticleCount);
	}

	void RenderParticles() {
		shader_->Use();

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, Globals::maxParticleCount * sizeof(GLfloat)* 4, positionBuffer_);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[2]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, Globals::maxParticleCount * sizeof(GLubyte)* 4, colorBuffer_);

		glActiveTexture(GL_TEXTURE0);
		texture_->Use();
		shader_->SetUniform("textureSampler", 0);

		const glm::mat4& proj = camera_->GetProjMatrix();
		const glm::mat4& view = camera_->GetViewMatrix();

		glm::mat4 VP = proj * view;
		glm::vec3 cameraRight_worldspace(view[0][0], view[1][0], view[2][0]);
		glm::vec3 cameraUp_worldspace(view[0][1], view[1][1], view[2][1]);

		shader_->SetUniform("VP", &VP);
		shader_->SetUniform("cameraRight_worldspace", &cameraRight_worldspace);
		shader_->SetUniform("cameraUp_worldspace", &cameraUp_worldspace);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[2]);
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, nullptr);

		glVertexAttribDivisor(0, 0);
		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, activeParticleCount_);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}

	void UpdateParticles(float deltaTime) {
		const glm::mat4& view = camera_->GetViewMatrix();

		// ???
		glm::vec3 cameraPosition(glm::inverse(view)[3]);

		int index = 0;
		for (int i = 0; i < Globals::maxParticleCount; ++i) {
			Particle& p = particles_[i];
			if (p.life < 0) {
				continue;
			}

			if ((p.life -= deltaTime) > 0) {
				p.speed += glm::vec3(0, -9.81f, 0) * deltaTime * 0.5f;
				p.position += p.speed * deltaTime;
				p.cameraDistance2 = glm::length2(p.position - cameraPosition);

				positionBuffer_[4 * index + 0] = p.position.x;
				positionBuffer_[4 * index + 1] = p.position.y;
				positionBuffer_[4 * index + 2] = p.position.z;
				positionBuffer_[4 * index + 3] = p.size;

				colorBuffer_[4 * index + 0] = p.r;
				colorBuffer_[4 * index + 1] = p.g;
				colorBuffer_[4 * index + 2] = p.b;
				colorBuffer_[4 * index + 3] = p.a;
			}
			else {
				p.cameraDistance2 = -1.f;
			}

			++index;
		}

		activeParticleCount_ = index;
	}

	void CreateNewParticles(float deltaTime) {
		int count = glm::min(int(deltaTime * Globals::maxParticleCount), Globals::maxParticlePerFrame);
		for (int i = 0; i < count; ++i) {
			int index = FindUnusedParticle();
			Particle& p = particles_[index];
			p.life = 5.f;
			p.position = glm::vec3(0, 0, -20.f);

			glm::vec3 randomdir(
				(rand() % 2000 - 1000.f) / 1000.f,
				(rand() % 2000 - 1000.f) / 1000.f,
				(rand() % 2000 - 1000.f) / 1000.f
				);

			float spread = 1.5f;
			glm::vec3 maindir(0, 10.f, 0);
			particles_[i].speed = maindir + randomdir * spread;

			// Very bad way to generate a random color
			p.r = rand() % 256;
			p.g = rand() % 256;
			p.b = rand() % 256;
			p.a = (rand() % 256) / 3;

			p.size = (rand() % 1000) / 4000.0f + 0.1f;
		}
	}

private:
	Texture* texture_;

	Particle* particles_;

	int lastUsedParticle_ = 0;
	int activeParticleCount_ = 0;

	GLuint vao_;
	GLuint vbo_[3];

	GLubyte* colorBuffer_;
	GLfloat* positionBuffer_;
};
