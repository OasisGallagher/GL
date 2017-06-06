#include <assimp/postprocess.h>

#include "mesh.h"
#include "debug.h"
#include "texture.h"

MeshEntry::MeshEntry() {
	ib = vb = GL_INVALID_INDEX;
	numIndices = 0;
	materialIndex = GL_INVALID_INDEX;
}

MeshEntry::~MeshEntry() {
	if (vb != GL_INVALID_INDEX) {
		glDeleteBuffers(1, &vb);
	}

	if (ib != GL_INVALID_INDEX) {
		glDeleteBuffers(1, &ib);
	}
}

bool MeshEntry::Init(const std::vector<MeshVertex>& vertices, const std::vector<unsigned>& indices) {
	numIndices = indices.size();

	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)* numIndices, &indices[0], GL_STATIC_DRAW);

	return true;
}

Mesh::Mesh() {
	vao_ = 0;
}

Mesh::~Mesh() {
	Clear();
}

void Mesh::Clear() {
	for (unsigned i = 0; i < textures_.size(); ++i) {
		delete textures_[i];
	}

	textures_.clear();
	glDeleteVertexArrays(1, &vao_);
}

bool Mesh::Load(const std::string& path) {
	Clear();

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	Assimp::Importer importer;
	unsigned flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
	const aiScene* scene = importer.ReadFile(path.c_str(), flags);

	Assert(scene != nullptr, "failed to read file " + path + ": " + importer.GetErrorString());
	return InitFromScene(scene, path);
}

bool Mesh::InitFromScene(const aiScene* scene, const std::string& path) {
	entries_.resize(scene->mNumMeshes);
	textures_.resize(scene->mNumMaterials);

	for (unsigned i = 0; i < entries_.size(); ++i) {
		InitMesh(i, scene->mMeshes[i]);
	}

	return InitMaterials(scene, path);
}

void Mesh::InitMesh(unsigned index, const aiMesh* mesh) {
	entries_[index].materialIndex = mesh->mMaterialIndex;

	std::vector<MeshVertex> vertices;
	std::vector<unsigned> indices;

	const aiVector3D zero(0);
	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		const aiVector3D* pos = &mesh->mVertices[i];
		const aiVector3D* normal = &mesh->mNormals[i];
		const aiVector3D* texCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &zero;
		const aiVector3D* tangent = &mesh->mTangents[i];

		MeshVertex v;
		v.position.x = pos->x;
		v.position.y = pos->y;
		v.position.z = pos->z;

		v.uv.x = texCoord->x;
		v.uv.y = texCoord->y;

		v.normal.x = normal->x;
		v.normal.y = normal->y;
		v.normal.z = normal->z;

		v.tangent.x = tangent->x;
		v.tangent.y = tangent->y;
		v.tangent.z = tangent->z;

		vertices.push_back(v);
	}

	for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
		const aiFace& face = mesh->mFaces[i];
		Assert(face.mNumIndices == 3, "invalid index count");
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	entries_[index].Init(vertices, indices);
}

bool Mesh::InitMaterials(const aiScene* scene, const std::string& path) {
	bool success = true;
	for (unsigned i = 0; i < scene->mNumMaterials; ++i) {
		const aiMaterial* material = scene->mMaterials[i];
		textures_[i] = nullptr;

		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString dpath;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &dpath) == AI_SUCCESS) {
				std::string fullPath = "textures/";
				fullPath += dpath.data;
				textures_[i] = new Texture;
				if (!textures_[i]->Load(fullPath)) {
					success = false;
					delete textures_[i];
					textures_[i] = nullptr;
				}
			}
		}
	}

	return std::find(textures_.begin(), textures_.end(), nullptr) == textures_.end();
}

void Mesh::Render(GLenum mode) {
	glBindVertexArray(vao_);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	//glEnableVertexAttribArray(3);

	for (unsigned i = 0; i < entries_.size(); ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, entries_[i].vb);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (const void*)12);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (const void*)20);
		//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (const void*)32);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entries_[i].ib);
		/*
		unsigned mi = entries_[i].materialIndex;
		if (mi < textures_.size() && textures_[i] != nullptr) {
			glActiveTexture(GL_TEXTURE0);
			textures_[i]->Use();
		}
		*/
		glDrawElements(mode, entries_[i].numIndices, GL_UNSIGNED_INT, nullptr);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	//glDisableVertexAttribArray(3);
}
