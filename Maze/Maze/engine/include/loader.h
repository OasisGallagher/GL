#pragma once
#include <vector>
#include <glm/glm.hpp>

class TextLoader {
public:
	static bool Load(const std::string& file, std::string& text);
};


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

class VBOIndexer {
	struct VBOBuffer {
		glm::vec3 vertex;
		glm::vec2 uv;
		glm::vec3 normal;

		bool operator < (const VBOBuffer& other) const {
			return memcmp(this, &other, sizeof(other)) < 0;
		}
	};
public:
	static void Index(ModelInfo& packed, std::vector<unsigned>& indices, const ModelInfo& info);
};
