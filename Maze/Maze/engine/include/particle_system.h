#pragma once
#include <gl/glew.h>
#include <glm/glm.hpp>

#include "texture.h"

class Shader;
class Texture2D;
class RandomTexture;

class ParticleSystem {
	struct Particle {
		int type;
		glm::vec3 position;
		glm::vec3 velocity;
		float lifeTime;
	};

public:
	ParticleSystem();
	~ParticleSystem();

public:
	bool Init(const glm::vec3& position);
	void Render(float deltaTime, const glm::mat4& VP, const glm::vec3& cameraPosition);

private:
	void InitUpdateShader();
	void InitBillboardShader();

	void UpdateParticles(float deltaTime);
	void RenderParticles(const glm::mat4& VP, const glm::vec3& cameraPosition);

private:
	bool isFirst_;
	unsigned currentVB_;
	unsigned currentTFB_;
	GLuint vao_;
	GLuint particleBuffer_[2];
	GLuint transformFeedback_[2];

	float time_;

	Shader* updateShader_;
	Shader* billboardShader_;

	Texture2D* texture_;
	RandomTexture* randomTexture_;

	static const int kMaxParticles = 1000;
};
