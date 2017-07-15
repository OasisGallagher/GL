#pragma once
#include <glm/glm.hpp>

enum KeyCode {
	KeyCodeSpace,

	KeyCodeForward,
	KeyCodeBackward,
	KeyCodeLeft,
	KeyCodeRight,

	KeyCodeCount,
};

enum MouseButton {
	LeftButton,
	RightButton,
	MiddleButton,
};

class Input {
public:
	virtual void Update() = 0;
	virtual bool IsKeyUp(KeyCode key) = 0;
	virtual bool IsKeyDown(KeyCode key) = 0;
	virtual bool IsMouseButtonPressed(MouseButton button) = 0;
	virtual glm::vec2 GetCursorPosition() = 0;
};

struct GLFWwindow;

class GLFWInput : public Input {
public:
	GLFWInput(GLFWwindow* window);

public:
	virtual void Update();
	virtual bool IsKeyUp(KeyCode key);
	virtual bool IsKeyDown(KeyCode key);
	virtual bool IsMouseButtonPressed(MouseButton button);
	virtual glm::vec2 GetCursorPosition();

private:
	bool keyPress_[2][KeyCodeCount];
	GLFWwindow* window_;
};
