#pragma once
#include <glm/glm.hpp>

class WindowInput {
public:
	WindowInput();

public:
	const glm::mat4& GetViewMatrix() const {
		return view_;
	}

	const glm::mat4 GetProjectionMatrix() const {
		return proj_;
	}

public:
	virtual void Update(void* ptr, float deltaTime) = 0;

protected:
	glm::vec3 position_;
	float horizontalAngle_;
	float verticalAngle_;
	float fov_;
	float speed_, mouseSpeed_;
	glm::mat4 view_, proj_;
};

class GLFWInput : public WindowInput {
public:
	virtual void Update(void* ptr, float deltaTime);
};

class Input {
public:
	static void Update(void* ptr, float deltaTime) {
		return input_->Update(ptr, deltaTime);
	}

	static const glm::mat4& GetViewMatrix() {
		return input_->GetViewMatrix();
	}

	static const glm::mat4 GetProjectionMatrix() {
		return input_->GetProjectionMatrix();
	}

private:
	Input();

private:
	static WindowInput* input_;
};
