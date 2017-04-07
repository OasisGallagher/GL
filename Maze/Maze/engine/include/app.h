#pragma once

#include <glm/glm.hpp>

class Example;
struct GLFWwindow;

struct AppEnv {
	bool depthTest;
	glm::vec4 backgroundColor;
};

class App {
public:
	bool Initialize();
	void Destroy();

public:
	void SetExample(Example* example);
	void EnterMainLoop();

private:
	AppEnv env_;

	GLFWwindow* window_;
	Example* example_;
};