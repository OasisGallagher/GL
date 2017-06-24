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

class Example {
public:
	Example() {
		shader_ = new Shader;
		camera_ = new Camera;

		lpressed_ = rpressed_ = false;
	}

	virtual ~Example() {
		delete shader_;
		delete camera_;
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
		glm::vec2 delta(0);
		Input* input = app.GetInput();
		bool pressed = input->IsMouseButtonPressed(LeftButton);
		if (pressed && (delta = GetMouseDragDelta(lpressed_)) != glm::zero<glm::vec2>()) {
			camera_->Rotate(delta);
		}

		lpressed_ = pressed;

		pressed = input->IsMouseButtonPressed(RightButton);
		if (pressed && (delta = GetMouseDragDelta(rpressed_)) != glm::zero<glm::vec2>()) {
			camera_->Zoom(-delta.y);
		}

		rpressed_ = pressed;

		pressed = input->IsMouseButtonPressed(MiddleButton);
		if (pressed && (delta = GetMouseDragDelta(mpressed_)) != glm::zero<glm::vec2>()) {
			delta.y = -delta.y;
			camera_->Move(-delta);
		}

		mpressed_ = pressed;
	}

	glm::vec2 GetMouseDragDelta(bool pressed0) {
		glm::vec2 ans;
		glm::vec2 pos = app.GetInput()->GetCursorPosition();

		if (pressed0 && mouse_ != pos) {
			ans = pos - mouse_;
			ans.y = -ans.y;
		}

		mouse_ = pos;

		return ans;
	}

protected:
	Camera* camera_;
	Shader* shader_;

	glm::vec2 mouse_;
	bool lpressed_, rpressed_, mpressed_;
};
