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

enum MouseButton {
	LeftButton,
	RightButton,
	MiddleButton,
};

class Input {
public:
	virtual bool IsKeyDown(KeyCode key) = 0;
	virtual bool IsMouseButtonPressed(MouseButton button) = 0;
	virtual glm::vec2 GetCursorPosition() = 0;
};

struct GLFWwindow;

class GLFWInput : public Input {
public:
	GLFWInput(GLFWwindow* window);

public:
	virtual bool IsKeyDown(KeyCode key);
	virtual bool IsMouseButtonPressed(MouseButton button);
	virtual glm::vec2 GetCursorPosition();

private:
	GLFWwindow* window_;
};
