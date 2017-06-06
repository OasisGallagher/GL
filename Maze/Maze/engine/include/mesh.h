#pragma once

#include <vector>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

class Texture;

struct MeshVertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	glm::vec3 tangent;
};

struct MeshEntry {
	MeshEntry();
	~MeshEntry();

	bool Init(const std::vector<MeshVertex>& vertices, const std::vector<unsigned>& indices);

	GLuint vb, ib;
	unsigned numIndices;
	unsigned materialIndex;
};

class Mesh {
public:
	Mesh();
	~Mesh();

public:
	bool Load(const std::string& path);
	void Render(GLenum mode = GL_TRIANGLES);

private:
	bool InitFromScene(const aiScene* scene, const std::string& path);
	void InitMesh(unsigned index, const aiMesh* mesh);
	bool InitMaterials(const aiScene* scene, const std::string& path);
	void Clear();

private:
	GLuint vao_;

	std::vector<MeshEntry> entries_;
	std::vector<Texture*> textures_;
};
