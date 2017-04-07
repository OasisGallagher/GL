#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>

#include "app.h"
#include "input.h"
#include "debug.h"
#include "example.h"

bool App::Initialize() {
	// Initialise GLFW
	if (!glfwInit())
	{
		Debug::LogError("failed to initialize GLFW\n");
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window_ = glfwCreateWindow(512, 384, "Maze", NULL, NULL);
	if (window_ == NULL){
		Debug::LogError("failed to open GLFW window.");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window_);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		Debug::LogError("failed to initialize GLEW\n");
		glfwTerminate();
		return false;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);

	return true;
}

void App::SetExample(Example* example) {
	example_ = example;
	example->GetEnvRequirement(env_);

	glm::vec4& bgColor = env_.backgroundColor;
	glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);

	if (env_.depthTest) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
}

void App::EnterMainLoop() {
	clock_t lastFrame = clock();

	do{
		clock_t now = clock();
		float deltaTime = float(now - lastFrame) / CLOCKS_PER_SEC;
		lastFrame = now;

		GLbitfield mask = GL_COLOR_BUFFER_BIT;
		if (env_.depthTest) {
			mask |= GL_DEPTH_BUFFER_BIT;
		}

		glClear(mask);

		Input::Update(window_, deltaTime);
		example_->Update(deltaTime);

		glfwSwapBuffers(window_);
		glfwPollEvents();
	} while (glfwGetKey(window_, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window_) == 0);
}

void App::Destroy() {
	glfwTerminate();
}
