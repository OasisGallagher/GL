#include <vector>

#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "app.h"

class Input;
class Shader;
class Camera;
class Text2D;
class Texture;
struct ModelInfo;
class RenderTexture;

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

class Example_VBOIndexing : public Example {
public:
	Example_VBOIndexing();
	~Example_VBOIndexing();

public:
	virtual void Update(float deltaTime);
	virtual void GetEnvRequirement(AppEnv& env);

private:

	Text2D* text_;

	ModelInfo* info_;
	Texture* texture_;
	GLuint vao_, vbo_[4];
	std::vector<unsigned> indices_;
};

class Example_NormalMapping : public Example {
public:
	Example_NormalMapping();
	~Example_NormalMapping();

public:
	virtual void Update(float deltaTime);
	virtual void GetEnvRequirement(AppEnv& env);

private:
	ModelInfo* modelInfo_;
	std::vector<unsigned> indices_;

	Texture* normal_;
	Texture* diffuse_, *specular_;

	GLuint vao_;
	GLuint vertexVbo_, uvVbo_, indexVbo_, normalVbo_;
	GLuint tangentVbo_, bitangentVbo_;
};


class Example_RenderToTexture : public Example {
public:
	Example_RenderToTexture();
	~Example_RenderToTexture();

public:
	virtual void Update(float deltaTime);

private:
	ModelInfo* modelInfo_;
	Texture* texture_;

	Shader* shader2_;

	RenderTexture* renderTexture_;

	GLuint vao_;
	GLuint vbo_[4];

	float elapsed_;
};

class Example_LightMaps : public Example {
public:
	Example_LightMaps();
	~Example_LightMaps();

public:
	virtual void Update(float deltaTime);
	virtual void GetEnvRequirement(AppEnv& env);

private:
	GLuint vao_;
	GLuint vbo_[2];

	Texture* texture_;
	ModelInfo* modelInfo_;
};

class Example_ShadowMaps : public Example {
public:
	Example_ShadowMaps();
	~Example_ShadowMaps();

public:
	virtual void Update(float deltaTime);

	void RenderPass();

	void RenderShadowMap();

	void ShadowMapPass();

	virtual void GetEnvRequirement(AppEnv& env);

private:
	GLuint vao_;
	GLuint vbo_[5];
	std::vector<unsigned> indices_;

	glm::mat4 shadowMVP_;

	Texture* texture_;
	ModelInfo* modelInfo_;

	Shader* depthShader_;
	Shader* shadowShader_;

	RenderTexture* depthTexture_;
};
