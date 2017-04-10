#include <fstream>

#include "debug.h"
#include "reader.h"

bool Reader::ReadAllText(const std::string& file, std::string& text) {
	std::ifstream ifs(file, std::ios::in);
	if (!ifs) {
		Debug::LogError("failed to open file " + file + ".");
		return false;
	}

	text.clear();
	const char* seperator = "";

	std::string line;
	for (; getline(ifs, line);) {
		text += seperator;
		text += line;
		seperator = "\n";
	}

	ifs.close();

	return true;
}

bool ModelLoader::Load(const std::string& path, ModelInfo& info) {
	FILE* file = fopen(path.c_str(), "r");
	if (file == nullptr) {
		Debug::LogError("invalid file path + " + path + ".");
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

	for (;;) {
		if (fscanf(file, "%s", header) == EOF) {
			break;
		}

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
			   vertexIndex, uvIndex, normalIndex,
			   vertexIndex + 1, uvIndex + 1, normalIndex + 1,
			   vertexIndex + 2, uvIndex + 1, normalIndex + 2
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
