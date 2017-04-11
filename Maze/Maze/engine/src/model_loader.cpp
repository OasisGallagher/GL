#include <map>

#include "debug.h"
#include "model_loader.h"

bool ModelLoader::Load(const std::string& path, ModelInfo& info){
	std::string::size_type i = path.rfind('.');
	if (i == std::string::npos) {
		Debug::LogError("invalid file path " + path + ".");
		return false;
	}

	const char* ptr = path.c_str() + i + 1;
	if (strcmp("obj", ptr) == 0) {
		return LoadBlenderObj(path, info);
	}

	Debug::LogError("invalid postfix " + path.substr(i) + ".");
	return false;
}

bool ModelLoader::LoadBlenderObj(const std::string& path, ModelInfo& info) {
	FILE* file = fopen(path.c_str(), "r");
	if (file == nullptr) {
		Debug::LogError("invalid file path " + path + ".");
		return false;
	}

	glm::vec2 uv;
	glm::vec3 vertex, normal;

	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> vertices, normals;

	unsigned vertexIndex[3], uvIndex[3], normalIndex[3];
	std::vector<unsigned> vertexIndices, uvIndices, normalIndices;

	char header[128];
	char dummy[512];
	
	for (;!feof(file);) {
		fscanf(file, "%s", header);

		if (strcmp(header, "v") == 0) {
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);
		}
		else if (strcmp(header, "vt") == 0) {
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uvs.push_back(uv);
		}
		else if (strcmp(header, "vn") == 0) {
			fscanf(file, "%f %f %f", &normal.x, &normal.y, &normal.z);
			normals.push_back(normal);
		}
		else if (strcmp(header, "f") == 0) {
			fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]
				);

			vertexIndices.insert(vertexIndices.end(), vertexIndex, vertexIndex + 3);
			uvIndices.insert(uvIndices.end(), uvIndex, uvIndex + 3);
			normalIndices.insert(normalIndices.end(), normalIndex, normalIndex + 3);
		}
		else {
			fgets(dummy, sizeof(dummy), file);
		}
	}

	for (unsigned i = 0; i < vertexIndices.size(); ++i) {
		unsigned vi = vertexIndices[i];
		unsigned ui = uvIndices[i];
		unsigned ni = normalIndices[i];

		info.vertices.push_back(vertices[vi - 1]);
		info.uvs.push_back(uvs[ui - 1]);
		info.normals.push_back(normals[ni - 1]);
	}

	return true;
}

void VBOIndexer::Index(ModelInfo& packed, std::vector<unsigned>& indices, const ModelInfo& info) {
	ModelInfo newInfo;
	typedef std::map<VBOBuffer, unsigned> Container;
	Container dict;
	unsigned index = 0;
	unsigned size = info.vertices.size();

	for (unsigned i = 0; i < size; ++i) {
		VBOBuffer buffer = { info.vertices[i], info.uvs[i], info.normals[i] };
		Container::iterator pos = dict.find(buffer);
		if (pos == dict.end()) {
			dict.insert(std::make_pair(buffer, index));

			newInfo.vertices.push_back(buffer.vertex);
			newInfo.uvs.push_back(buffer.uv);
			newInfo.normals.push_back(buffer.normal);

			indices.push_back(index);
			++index;
		}
		else {
			indices.push_back(pos->second);
		}
	}

	packed = newInfo;
}
