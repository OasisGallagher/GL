#pragma once

#include <glm/glm.hpp>

class Input;
class Example;
struct GLFWwindow;

struct AppEnv {
	bool blend;
	bool cullFace;
	bool depthTest;
	glm::vec4 backgroundColor;
};

class App {
public:
	bool Initialize();
	void Destroy();

public:
	Input* GetInput();
	GLFWwindow* GetWindow();

	void GetWindowSize(int& width, int& height);
	void SetExample(Example* example);
	void EnterMainLoop();

private:
	AppEnv env_;

	Input* input_;
	GLFWwindow* window_;
	Example* example_;
};