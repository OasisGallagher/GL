#include <cstdio>
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>

#include "shader.h"

#pragma comment(lib, "external/glew-1.9.0/bin/glew32.lib")
#pragma comment(lib, "external/glfw-3.1.2/bin/glfw3.lib")
#pragma comment(lib, "opengl32.lib")

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}	
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Open a window and create its OpenGL context
	GLFWwindow* window = glfwCreateWindow(1024, 768, "Tutorial 01", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);	const GLfloat vertexBufferData[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};	GLuint vao;	glGenVertexArrays(1, &vao);	glBindVertexArray(vao);	GLuint vertexBuffer;	glGenBuffers(1, &vertexBuffer);	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);	glBufferData(GL_VERTEX_ARRAY, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);	Shader shader;	shader.Load(ShaderTypeVertex, "shaders/simple.vert");	shader.Load(ShaderTypeFragment, "shaders/simple.frag");
	do{
		// Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
		glClear(GL_COLOR_BUFFER_BIT);		shader.Link();		glEnableVertexAttribArray(0);		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);		glDrawArrays(GL_TRIANGLES, 0, 3);		glDisableVertexAttribArray(0);
		// Draw nothing, see you in tutorial 2 !
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteVertexArrays(1, &vao);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();	
	return 0;
}

