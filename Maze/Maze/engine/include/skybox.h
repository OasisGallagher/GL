#pragma once
#include <string>
#include <gl/glew.h>

#include "loader.h"

class Shader;
class Camera;
struct ModelInfo;
class CubemapTexture;

struct SkyBoxInitParameter {
	Camera* camera;
	std::string posx; std::string negx;
	std::string posy; std::string negy;
	std::string posz; std::string negz;
};

class SkyBox {
public:
	SkyBox(const SkyBoxInitParameter& p);
	~SkyBox();

public:
	void Render();

private:
	GLuint vao_;
	GLuint vbo_[1];

	Shader* shader_;
	CubemapTexture* cubemapTexture_;

	ModelInfo* modelInfo_;

	SkyBoxInitParameter p_;
};
