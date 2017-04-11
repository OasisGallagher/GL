#pragma once

#include <glm/glm.hpp>

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
	GLFWwindow* GetWindow() { return window_; }
	void GetWindowSize(int& width, int& height);

	void SetExample(Example* example);
	void EnterMainLoop();

private:
	AppEnv env_;

	GLFWwindow* window_;
	Example* example_;
};