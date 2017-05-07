#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>

#include "app.h"
#include "defs.h"
#include "input.h"
#include "debug.h"
#include "example.h"

void APIENTRY DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam) {

	std::string text = "OpenGL Debug Output message:\n";

	if (source == GL_DEBUG_SOURCE_API_ARB)					text += "Source: API.\n";
	else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)	text += "Source: WINDOW_SYSTEM.\n";
	else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)	text += "Source: SHADER_COMPILER.\n";
	else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)		text += "Source: THIRD_PARTY.\n";
	else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)		text += "Source: APPLICATION.\n";
	else if (source == GL_DEBUG_SOURCE_OTHER_ARB)			text += "Source: OTHER.\n";

	if (type == GL_DEBUG_TYPE_ERROR_ARB)					text += "Type: ERROR.\n";
	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)	text += "Type: DEPRECATED_BEHAVIOR.\n";
	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)	text += "Type: UNDEFINED_BEHAVIOR.\n";
	else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)			text += "Type: PORTABILITY.\n";
	else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)			text += "Type: PERFORMANCE.\n";
	else if (type == GL_DEBUG_TYPE_OTHER_ARB)				text += "Type: OTHER.\n";

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)				text += "Severity: HIGH.\n";
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)		text += "Severity: MEDIUM.\n";
	else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)			text += "Severity: LOW.\n";

	text += message;

	Assert(severity != GL_DEBUG_SEVERITY_HIGH_ARB, text);

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB) {
		Debug::LogError(text);
	}
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB){
		Debug::LogWarning(text);
	}
	else {
		Debug::Log(text);
	}
}

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

	// ARB_debug_output is a bit special, 
	// it requires creating the OpenGL context
	// with paticular flags.
	// GLFW exposes it this way; if you use SDL, SFML, freeGLUT
	// or other, check the documentation.
	// If you use custom code, read the spec : 
	// http://www.opengl.org/registry/specs/ARB/debug_output.txt
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

	// Open a window and create its OpenGL context
	window_ = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Maze", nullptr, nullptr);
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

	if (GLEW_ARB_debug_output) {
		glDebugMessageCallbackARB(DebugOutputCallback, nullptr);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);

	return true;
}

void App::GetWindowSize(int& width, int& height) {
	glfwGetWindowSize(window_, &width, &height);
}

void App::SetExample(Example* example) {
	example_ = example;
	example->GetEnvRequirement(env_);

	glm::vec4& bgColor = env_.backgroundColor;
	glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);

	if (env_.blend) {
		// color = colorCurrent * alphaCurrent + colorOut * (1 - alphaCurrent).
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		glDisable(GL_BLEND);
	}

	if (env_.cullFace)  {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

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

		example_->Update(deltaTime);

		glfwSwapBuffers(window_);
		glfwPollEvents();
	} while (glfwGetKey(window_, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window_) == 0);
}

void App::Destroy() {
	//glfwTerminate();
}
