#pragma once
#include <glm/glm.hpp>

class Texture2D;

class Decal {
public:
	Decal();
	~Decal();

public:
	void SetProjector(const glm::mat4& mvp);
	void SetTexture(Texture2D* texture);

public:
	void Render();

private:
	Texture2D* texture_;
};
