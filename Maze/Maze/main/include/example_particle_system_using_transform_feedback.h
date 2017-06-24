#pragma once
#include "example.h"
#include "particle_system.h"

class Example_ParticleSystemUsingTransformFeedback : public Example {
public:
	Example_ParticleSystemUsingTransformFeedback() {
		ps_ = new ParticleSystem;

		camera_->Reset(glm::vec3(0, 0.4f, -5.f), glm::vec3(0, 0.2f, 1.f));
		ps_->Init(glm::vec3(0, 1, 1.f));
	}

	~Example_ParticleSystemUsingTransformFeedback() {
		delete ps_;
	}

public:
	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);
		glm::mat4 VP = camera_->GetProjMatrix() * camera_->GetViewMatrix();
		ps_->Render(deltaTime, VP, camera_->GetPosition());
	}

	virtual void GetEnvRequirement(AppEnv& env) {
		Example::GetEnvRequirement(env);
		env.blend = true;
	}

private:
	ParticleSystem* ps_;
};
