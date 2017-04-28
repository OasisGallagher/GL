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
	bool LoadCommon(const std::string& path);

	bool GetBmpData(const std::string& path, TextureData& td);
	bool GetDDSData(const std::string& path, TextureData& td);

	GLuint CreateFromBmp(const TextureData& td);
	GLuint CreateFromDDS(const TextureData& td);

	void Destroy();

	bool UseTexture();
private:
	GLuint textureID_;
};

class CubemapTexture {
public:
	CubemapTexture();
	~CubemapTexture();

public:
	bool Load(const std::string& posx, const std::string& negx,
		const std::string& posy, const std::string& negy,
		const std::string& posz, const std::string& negz
	);

	void Use();

private:
	void Destroy();
	GLuint CreateCubeTexture();

private:
	GLuint textureID_;
	std::string fileNames_[6];
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

class RandomTexture {
public:
	RandomTexture();
	~RandomTexture();

public:
	bool Load(unsigned size);
	void Use();

private:
	GLuint LoadRandomTexture(unsigned size);
	void Destroy();

private:
	GLuint textureID_;
};