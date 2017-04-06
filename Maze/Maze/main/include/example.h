#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "app.h"

class Shader;

class Example {
public:
	Example();
	virtual ~Example();

public:
	virtual void GetEnvRequirement(AppEnv& env);
	virtual void Update(float deltaTime) = 0;

protected:
	Shader* shader_;
};

class Example_RedTriangle : public Example {
public:
	Example_RedTriangle();
	~Example_RedTriangle();

public:
	virtual void Update(float deltaTime);

private:
	GLuint vao_, vbo_;
};

class Example_Matrices : public Example {
public:
	Example_Matrices();
	~Example_Matrices();

public:
	virtual void Update(float deltaTime);

private:
	GLuint vao_, vbo_;
	glm::mat4 mvp_;
};

class Example_ColoredCube : public Example {
public:
	Example_ColoredCube();
	~Example_ColoredCube();

public:
	virtual void Update(float deltaTime);
	virtual void GetEnvRequirement(AppEnv& env);

private:
	GLuint vao_, vbo_[2];
};