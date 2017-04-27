#pragma once

#include <vector>
#include <gl/glfw3.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>

struct MeshVertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};

class Mesh {
public:
	Mesh();
	~Mesh();

public:
	bool Load(const std::string& path);
	void Render();

};
