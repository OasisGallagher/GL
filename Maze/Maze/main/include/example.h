#pragma once

#include <vector>

#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "app.h"

#include "defs.h"
#include "debug.h"
#include "mathf.h"
#include "input.h"
#include "shader.h"
#include "camera.h"
#include "utilities.h"

extern App app;

namespace Globals {
	static const float defaultAspect = 4.f / 3.f;
	static const float moveSpeed = 3.f;

	static const int maxParticleCount = 10000;
	static const int maxParticlePerFrame = int(0.016 * maxParticleCount);

	static const GLfloat quadData[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};
};

class Example {
public:
	Example() {
		shader_ = new Shader;
		camera_ = new Camera;
		input_ = new GLFWInput(app.GetWindow());
	}

	virtual ~Example() {
		delete shader_;
		delete camera_;
		delete input_;
	}

public:
	virtual void GetEnvRequirement(AppEnv& env) {
		env.blend = false;
		env.cullFace = false;
		env.depthTest = true;
		env.backgroundColor = glm::vec4(0.0f, 0.0f, 0.4f, 0.0f);
	}

	virtual void Update(float deltaTime) {
		UpdateCamera(deltaTime);
	}

private:
	void UpdateCamera(float deltaTime) {
		if (input_->IsKeyDown(KeyCodeForward)) {
			camera_->Walk(-deltaTime * Globals::moveSpeed);
		}

		if (input_->IsKeyDown(KeyCodeBackward)) {
			camera_->Walk(deltaTime * Globals::moveSpeed);
		}

		if (input_->IsKeyDown(KeyCodeLeft)) {
			camera_->Strafe(-deltaTime * Globals::moveSpeed);
		}

		if (input_->IsKeyDown(KeyCodeRight)) {
			camera_->Strafe(deltaTime * Globals::moveSpeed);
		}

		if (input_->IsKeyDown(KeyCodePitchClockwise)) {
			camera_->Pitch(deltaTime * Mathf::PI / 50);
		}

		if (input_->IsKeyDown(KeyCodePitchAnticlockwise)) {
			camera_->Pitch(-deltaTime * Mathf::PI / 50);
		}

		if (input_->IsKeyDown(KeyCodeYawClockwise)) {
			camera_->Yaw(deltaTime * Mathf::PI / 50);
		}

		if (input_->IsKeyDown(KeyCodeYawAnticlockwise)) {
			camera_->Yaw(-deltaTime * Mathf::PI / 50);
		}

		if (input_->IsKeyDown(KeyCodeRollClockwise)) {
			camera_->Roll(deltaTime * Mathf::PI / 50);
		}

		if (input_->IsKeyDown(KeyCodeRollAnticlockwise)) {
			camera_->Roll(-deltaTime * Mathf::PI / 50);
		}
	}

protected:
	Input* input_;
	Camera* camera_;
	Shader* shader_;
};
