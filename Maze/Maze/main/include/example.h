#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "app.h"

class Input;
class Shader;
class Camera;
class Texture;
struct ModelInfo;

class Example {
public:
	Example();
	virtual ~Example();

public:
	virtual void GetEnvRequirement(AppEnv& env);
	virtual void Update(float deltaTime) = 0;

protected:
	Input* input_;
	Camera* camera_;
	Shader* shader_;
};

class Example_RedTriangle : public Example {
public:
	Example_RedTriangle();
	~Example_RedTriangle();

public:
	virtual void Update(float deltaTime);
	virtual void GetEnvRequirement(AppEnv& env);

private:
	GLuint vao_, vbo_;
};

class Example_Matrices : public Example {
public:
	Example_Matrices();
	~Example_Matrices();

public:
	virtual void Update(float deltaTime);
	virtual void GetEnvRequirement(AppEnv& env);

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

private:
	GLuint vao_, vbo_[2];
};

class Example_TexturedCube : public Example {
public:
	Example_TexturedCube();
	~Example_TexturedCube();

public:
	virtual void Update(float deltaTime);

private:
	glm::mat4 mvp_;

	Texture* texture0_, *texture1_;
	GLuint vao_, vbo_[2];
};

class Example_KeyboardAndMouse : public Example_TexturedCube {
public:
	Example_KeyboardAndMouse();
	~Example_KeyboardAndMouse();

public:
	virtual void Update(float deltaTime);

private:
	Shader* coordShader_;
	GLuint vao_, vbo_;
};

class Example_ModelLoading : public Example {
public:
	Example_ModelLoading();
	~Example_ModelLoading();

public:
	virtual void Update(float deltaTime);

private:
	ModelInfo* info_;
	Texture* texture_;
	GLuint vao_, vbo_[2];
};

class Example_BasicShading : public Example {
public:
	Example_BasicShading();
	~Example_BasicShading();

public:
	virtual void Update(float deltaTime);
	virtual void GetEnvRequirement(AppEnv& env);

private:
	ModelInfo* info_;
	Texture* texture_;
	GLuint vao_, vbo_[3];
};
