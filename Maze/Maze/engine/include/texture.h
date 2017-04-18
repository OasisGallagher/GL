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

enum RenderTarget {
	RenderTexture2D,
	RenderDepthTexture,
};

class RenderTexture {
public:
	RenderTexture(RenderTarget target, GLint width, GLint height);
	~RenderTexture();

public:
	void Use();
	GLuint GetTexture() const;

private:
	void Create(RenderTarget target, GLint width, GLint height);
	void CreateTexture2D(GLint width, GLint height);
	void CreateDepthTexture(GLint width, GLint height);

private:
	GLuint fbo_;

	GLuint depthBuffer_;
	GLuint targetTexture_;
};
