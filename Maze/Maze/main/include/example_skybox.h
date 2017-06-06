#pragma once
#include "skybox.h"
#include "example.h"

class Example_SkyBox : public Example {
public:
	Example_SkyBox() {
		SkyBoxInitParameter p;
		p.camera = camera_;
		p.posx = "textures/sp3right.jpg";
		p.negx = "textures/sp3left.jpg";
		p.posy = "textures/sp3top.jpg";
		p.negy = "textures/sp3bot.jpg";
		p.posz = "textures/sp3front.jpg";
		p.negz = "textures/sp3back.jpg";
		skyBox_ = new SkyBox(p);
	}

	~Example_SkyBox() {
		delete skyBox_;
	}

public:
	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);
		skyBox_->Render();
	}

private:
	SkyBox* skyBox_;
};
