#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

class Reader {
public:
	static bool ReadAllText(const std::string& file, std::string& text);
};

struct ModelInfo {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};

class ModelLoader {
public:
	static bool Load(const std::string& path, ModelInfo& info);
};
