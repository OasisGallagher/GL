#include <gl/glfw3.h>

#include "input.h"

GLFWInput::GLFWInput(GLFWwindow* window) : window_(window) {
	//glfwSetScrollCallback(window_);
}

bool GLFWInput::IsKeyDown(KeyCode key) {
	switch (key) {
	case KeyCodeForward:
		return glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS;

	case KeyCodeBackward:
		return glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS;

	case KeyCodeLeft:
		return glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS;

	case KeyCodeRight:
		return glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS;

	case KeyCodePitchClockwise:
		return glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS;

	case KeyCodePitchAnticlockwise:
		return glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS;

	case KeyCodeRollClockwise:
		return glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS;

	case KeyCodeRollAnticlockwise:
		return glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS;
	}

	return false;
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
