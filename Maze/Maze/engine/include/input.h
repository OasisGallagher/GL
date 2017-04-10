#pragma once
#include <glm/glm.hpp>

enum KeyCode {
	KeyCodeForward,
	KeyCodeBackward,
	KeyCodeLeft,
	KeyCodeRight,

	KeyCodePitchClockwise,
	KeyCodePitchAnticlockwise,
	KeyCodeYawClockwise,
	KeyCodeYawAnticlockwise,
	KeyCodeRollClockwise,
	KeyCodeRollAnticlockwise,
};

class Input {
public:
	virtual bool IsKeyDown(KeyCode key) = 0;
	virtual float GetMouseWheel() = 0;
};

struct GLFWwindow;

class GLFWInput : public Input {
public:
	GLFWInput(GLFWwindow* window);

public:
	virtual bool IsKeyDown(KeyCode key);
	virtual float GetMouseWheel();

private:
	static void OnMouseWheel(GLFWwindow* window, double x, double y);

private:
	GLFWwindow* window_;
};
