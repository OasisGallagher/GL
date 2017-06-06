#pragma once
#include "example.h"
#define GLFW_CDECL
#include <AntTweakBar.h>

class Example_AntTweakBar : public Example {
public:
	Example_AntTweakBar() {
		TwInit(TW_OPENGL_CORE, nullptr);
		TwWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

		TwBar* EulerGUI = TwNewBar("Euler settings");
		TwBar* QuaternionGUI = TwNewBar("Quaternion settings");
		TwSetParam(EulerGUI, nullptr, "refresh", TW_PARAM_CSTRING, 1, "0.1");
		TwSetParam(QuaternionGUI, nullptr, "position", TW_PARAM_CSTRING, 1, "300 16");

		glm::vec3 orientation;
		TwAddVarRW(EulerGUI, "Euler X", TW_TYPE_FLOAT, &orientation.x, "step=0.1");
		TwAddVarRW(EulerGUI, "Euler Y", TW_TYPE_FLOAT, &orientation.y, "step=0.1");
		TwAddVarRW(EulerGUI, "Euler Z", TW_TYPE_FLOAT, &orientation.z, "step=0.1");

		glm::vec3 position;
		TwAddVarRW(EulerGUI, "Pos X", TW_TYPE_FLOAT, &position.x, "step=0.1");
		TwAddVarRW(EulerGUI, "Pos Y", TW_TYPE_FLOAT, &position.y, "step=0.1");
		TwAddVarRW(EulerGUI, "Pos Z", TW_TYPE_FLOAT, &position.z, "step=0.1");

		glm::quat orientation2;
		TwAddVarRW(QuaternionGUI, "Quaternion", TW_TYPE_QUAT4F, &orientation2, "showval=true open=true");

		bool lookAtOther;
		TwAddVarRW(QuaternionGUI, "Use LookAt", TW_TYPE_BOOL8, &lookAtOther, "help='Look at the other monkey ?'");

		glfwSetMouseButtonCallback(app.GetWindow(), (GLFWmousebuttonfun)TwEventMouseButtonGLFW);
		glfwSetCursorPosCallback(app.GetWindow(), (GLFWcursorposfun)TwEventMousePosGLFW);
		glfwSetScrollCallback(app.GetWindow(), (GLFWscrollfun)TwEventMouseWheelGLFW);
		glfwSetKeyCallback(app.GetWindow(), (GLFWkeyfun)TwEventKeyGLFW);
		glfwSetCharCallback(app.GetWindow(), (GLFWcharfun)TwEventCharGLFW);
	}

	~Example_AntTweakBar() {
		TwTerminate();
	}

public:
	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);

		TwDraw();
	}
};
