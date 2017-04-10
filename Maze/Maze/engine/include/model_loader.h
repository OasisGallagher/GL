#pragma once
#include <vector>
#include <glm/glm.hpp>

struct ModelInfo {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};

class ModelLoader {
public:
	static bool Load(const std::string& path, ModelInfo& info);

private:
	static bool LoadBlenderObj(const std::string& path, ModelInfo& info);
};
