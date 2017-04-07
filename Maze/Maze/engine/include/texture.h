#pragma once
#include <string>
#include <vector>

#include <gl/glew.h>

typedef std::vector<unsigned char> bytes;

struct TextureData {
	GLsizei width;
	GLsizei height;

	bytes pixels;
	GLenum format;
	GLuint mipMapCount;
};

class Texture {
public:
	Texture();
	~Texture();

public:
	bool Use();
	bool Load(const std::string& path);

private:
	bool LoadBmp(const std::string& path);
	bool LoadDDS(const std::string& path);

	GLuint CreateFromBmp(const TextureData& td);
	GLuint CreateFromDDS(const TextureData& td);

	void Destroy();

	bool GetBmpData(const std::string& path, TextureData& td);
	bool GetDDSData(const std::string& path, TextureData& td);

	bool UseTexture();
private:
	GLuint textureID_;
};
