#include <gl/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "app.h"
#include "input.h"

static const float PI = 3.14f;

bool Input::enabled_ = false;
WindowInput* Input::input_ = new GLFWInput;

WindowInput::WindowInput() {
	position_ = glm::vec3(0, 0, 5);
	horizontalAngle_ = PI;
	verticalAngle_ = 0.f;
	fov_ = 45.f;

	speed_ = 3.f;
	mouseSpeed_ = 0.005f;
}

void GLFWInput::Update(void* ptr, float deltaTime) {
	GLFWwindow* window = (GLFWwindow*)ptr;

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	int height, width;
	glfwGetWindowSize(window, &width, &height);
	glfwSetCursorPos(window, width / 2.0, height / 2.0);

	horizontalAngle_ += mouseSpeed_ * float(width / 2.0 - xpos);
	verticalAngle_ += mouseSpeed_ * float(height / 2.0 - ypos);

	glm::vec3 dir(
		cos(verticalAngle_) * sin(horizontalAngle_),
		sin(verticalAngle_),
		cos(verticalAngle_) * cos(horizontalAngle_)
		);

	glm::vec3 right(
		sin(horizontalAngle_ - PI / 2.f),
		0,
		cos(horizontalAngle_ - PI / 2.f)
		);

	glm::vec3 up = glm::cross(right, dir);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		position_ += dir * deltaTime * speed_;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		position_ -= dir * deltaTime * speed_;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		position_ += dir * deltaTime * speed_;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		position_ -= dir * deltaTime * speed_;
	}

	float fov = fov_;
	proj_ = glm::perspective(fov, 4.f / 3.f, 0.1f, 100.f);
	view_ = glm::lookAt(position_, position_ + dir, up);
}
