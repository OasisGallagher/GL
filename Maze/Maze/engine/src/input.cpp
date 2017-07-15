#include <gl/glfw3.h>

#include "debug.h"
#include "input.h"
#include "utilities.h"

static int GLFWKeyCodes[] = {
	GLFW_KEY_SPACE,
	GLFW_KEY_W,
	GLFW_KEY_S,
	GLFW_KEY_A,
	GLFW_KEY_D
};

GLFWInput::GLFWInput(GLFWwindow* window) : window_(window) {
	//glfwSetScrollCallback(window_);
	Assert(COUNT_OF(GLFWKeyCodes) == KeyCodeCount, "invalid GLFWKeyCodes");
	memset(keyPress_, 0, sizeof(keyPress_));
}

void GLFWInput::Update() {
	for (int i = 0; i < COUNT_OF(keyPress_); ++i) {
		keyPress_[0][i] = keyPress_[1][i];
	}

	for (int i = 0; i < COUNT_OF(GLFWKeyCodes); ++i) {
		keyPress_[1][i] = glfwGetKey(window_, GLFWKeyCodes[i]) == GLFW_PRESS;
	}
}

bool GLFWInput::IsKeyUp(KeyCode key) {
	return keyPress_[0][key] && !keyPress_[1][key];
}

bool GLFWInput::IsKeyDown(KeyCode key) {
	return keyPress_[1][key];
}

bool GLFWInput::IsMouseButtonPressed(MouseButton button) {
	if (button == LeftButton) {
		return glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	}

	if (button == RightButton) {
		return glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	}

	return glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
}

glm::vec2 GLFWInput::GetCursorPosition() {
	double x, y;
	glfwGetCursorPos(window_, &x, &y);
	return glm::vec2((float)x, (float)y);
}
