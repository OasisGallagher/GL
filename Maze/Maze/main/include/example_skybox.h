#pragma once
#include "mesh.h"
#include "skybox.h"
#include "texture.h"
#include "example.h"

class Example_SkyBox : public Example {
public:
	Example_SkyBox() {
		std::string textures[] = {
			"textures/lake_skybox/right.jpg",
			"textures/lake_skybox/left.jpg",
			"textures/lake_skybox/top.jpg",
			"textures/lake_skybox/bottom.jpg",
			"textures/lake_skybox/back.jpg",
			"textures/lake_skybox/front.jpg",
		};

		skyBox_ = new SkyBox(camera_, textures);

		mesh_.Load("models/box.obj");

		reflect_.Load("shaders/reflect.shader");
		reflect_.Link();

		refract_.Load("shaders/refract.shader");
		refract_.Link();

		camera_->Reset(glm::vec3(0, 1, 5), glm::vec3(0));
	}

	~Example_SkyBox() {
		delete skyBox_;
	}

public:
	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);
		
		RenderReflect();		
		RenderRefract();

		skyBox_->Render();
	}	

private:
	void RenderRefract() {
		glm::mat4 m;
		m[3] = glm::vec4(1.3f, 0, 0, 1);
		glm::mat4 mvp = camera_->GetProjMatrix() * camera_->GetViewMatrix() * m;
		glm::mat4 mv = camera_->GetViewMatrix() * m;

		refract_.SetUniform("MV", &mv);
		refract_.SetUniform("MVP", &mvp);

		refract_.SetUniform("cube", Globals::ColorTextureIndex);
		skyBox_->GetTexture()->Bind(Globals::ColorTexture);

		refract_.Bind();
		mesh_.Render();
	}

	void RenderReflect() {
		glm::mat4 m;
		m[3] = glm::vec4(-1.3f, 0, 0, 1);

		glm::mat4 mvp = camera_->GetProjMatrix() * camera_->GetViewMatrix() * m;
		glm::mat4 mv = camera_->GetViewMatrix() * m;

		reflect_.SetUniform("MV", &mv);
		reflect_.SetUniform("MVP", &mvp);

		reflect_.SetUniform("cube", Globals::ColorTextureIndex);
		skyBox_->GetTexture()->Bind(Globals::ColorTexture);

		reflect_.Bind();
		mesh_.Render();
	}

private:
	Mesh mesh_;
	SkyBox* skyBox_;

	Shader reflect_;
	Shader refract_;
};
