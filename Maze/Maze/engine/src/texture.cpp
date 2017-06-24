#include <glm/glm.hpp>

#include "debug.h"
#include "texture.h"
#include "utilities.h"

#include "Magick++.h"

static const GLenum cubeMapTypes[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

Texture::Texture() : textureID_(0) {
}

Texture::~Texture() {
	Destroy();
}

bool Texture::Use() {
	if (!glIsTexture(textureID_)) {
		Debug::LogError("invalid texture.");
		return false;
	}

	return UseTexture();
}

bool Texture::Load(const std::string& path) {
	std::string::size_type i = path.rfind('.');
	if (i == std::string::npos) {
		Debug::LogError("invalid file path " + path + ".");
		return false;
	}

	const char* ptr = path.c_str() + i + 1;
	if (strcmp("bmp", ptr) == 0) {
		return LoadBmp(path);
	}

	if (strcmp("dds", ptr) == 0) {
		return LoadDDS(path);
	}

	return LoadCommon(path);
}

bool Texture::LoadCommon(const std::string& path) {
	Magick::Image image;
	Magick::Blob blob;
	try{
		image.read(path.c_str());
		image.write(&blob, "RGBA");
	}
	catch (Magick::Error& err) {
		Debug::LogError("failed to load " + path + ": " + err.what());
		return false;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.columns(), image.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, blob.data());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (textureID_ != 0) {
		Destroy();
	}

	textureID_ = textureID;
	return true;
}

bool Texture::LoadBmp(const std::string& path) {
	TextureData td;
	if (!GetBmpData(path, td)) {
		return false;
	}

	GLuint textureID = CreateFromBmp(td);
	if (textureID != 0) {
		Destroy();
		textureID_ = textureID;
		return true;
	}

	return false;
}

bool Texture::LoadDDS(const std::string& path) {
	TextureData td;
	if (!GetDDSData(path, td)) {
		return false;
	}

	GLuint textureID = CreateFromDDS(td);
	if (textureID != 0) {
		Destroy();
		textureID_ = textureID;
		return true;
	}

	return false;
}

bool Texture::UseTexture() {
	glBindTexture(GL_TEXTURE_2D, textureID_);
	return true;
}

GLuint Texture::CreateFromBmp(const TextureData& td) {
	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, td.width, td.height, 0, td.format, GL_UNSIGNED_BYTE, &td.pixels[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	return textureID;
}

GLuint Texture::CreateFromDDS(const TextureData& td) {
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (td.format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0, width = td.width, height = td.height;
		level < td.mipMapCount && (width != 0 || height != 0); ++level) {
		unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, td.format, width, height,
			0, size, &td.pixels[offset]);

		offset += size;
		width /= 2;
		height /= 2;

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if (width < 1) width = 1;
		if (height < 1) height = 1;

	}

	return textureID;
}

void Texture::Destroy() {
	if (textureID_ != 0) {
		glDeleteTextures(1, &textureID_);
		textureID_ = 0;
	}
}

bool Texture::GetBmpData(const std::string& path, TextureData& td) {
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;

	// Open the file
	FILE * file = fopen(path.c_str(), "rb");
	if (file == nullptr) {
		Debug::LogError(path + " could not be opened.");
		return false;
	}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if (fread(header, 1, 54, file) != 54){
		Debug::LogError("Not a correct BMP file.");
		return false;
	}
	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M'){
		Debug::LogError("Not a correct BMP file.");
		return false;
	}

	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0) {
		Debug::LogError("Not a correct BMP file.");   
		return false; 
	}

	if (*(int*)&(header[0x1C]) != 24) {
		Debug::LogError("Not a correct BMP file.");
		return false;
	}

	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0) {
		imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	}
	
	if (dataPos == 0) {
		dataPos = 54; // The BMP header is done that way
	}

	// Create a buffer
	td.pixels.resize(imageSize);
	// Read the actual data from the file into the buffer
	fread(&td.pixels[0], 1, imageSize, file);

	// Everything is in memory now, the file wan be closed
	fclose(file);

	td.format = GL_BGR;
	td.width = width;
	td.height = height;

	return true;
}

bool Texture::GetDDSData(const std::string& path, TextureData& td) {
	const int FOURCC_DXT1 = 0x31545844; // Equivalent to "DXT1" in ASCII
	const int FOURCC_DXT3 = 0x33545844; // Equivalent to "DXT3" in ASCII
	const int FOURCC_DXT5 = 0x35545844; // Equivalent to "DXT5" in ASCII

	unsigned char header[124];

	FILE *fp;

	/* try to open the file */
	fp = fopen(path.c_str(), "rb");
	if (fp == NULL) {
		Debug::LogError(path + " could not be opened");
		return false;
	}

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fclose(fp);
		return false;
	}

	/* get the surface desc */
	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);

	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	td.pixels.resize(bufsize);
	fread(&td.pixels[0], 1, bufsize, fp);
	/* close the file pointer */
	fclose(fp);

	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format = 0;
	switch (fourCC) {
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		break;
	}

	if (format == 0) {
		Debug::LogError("invalid format.");
		return false;
	}

	td.width = width;
	td.height = height;
	td.format = format;
	td.mipMapCount = mipMapCount;

	return true;
}

CubemapTexture::CubemapTexture() : textureID_(0) {
}

CubemapTexture::~CubemapTexture() {
	Destroy();
}

bool CubemapTexture::Load(const std::string& posx, const std::string& negx,
	const std::string& posy, const std::string& negy,
	const std::string& posz, const std::string& negz) {
	fileNames_[0] = posx;
	fileNames_[1] = negx;
	fileNames_[2] = posy;
	fileNames_[3] = negy;
	fileNames_[4] = posz;
	fileNames_[5] = negz;

	GLuint textureID = CreateCubeTexture();
	if (textureID != 0) {
		Destroy();
		textureID_ = textureID;
		return true;
	}

	return false;
}

GLuint CubemapTexture::CreateCubeTexture() {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (int i = 0; i < COUNT_OF(cubeMapTypes); ++i) {
		Magick::Image image;
		Magick::Blob blob;
		try{
			image.read(fileNames_[i].c_str());
			image.write(&blob, "RGBA");
		}
		catch (Magick::Error& err) {
			Debug::LogError("failed to load texture " + fileNames_[i] + ": " + err.what());
			return 0;
		}

		glTexImage2D(cubeMapTypes[i], 0, GL_RGBA, 
			image.columns(), image.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, blob.data());

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	return textureID;
}

void CubemapTexture::Use() {
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID_);
}

void CubemapTexture::Destroy() {
	if (textureID_ != 0) {
		glDeleteTextures(1, &textureID_);
		textureID_ = 0;
	}
}

RenderTexture::RenderTexture(RenderTarget target, GLint width, GLint height) {
	glGenFramebuffers(1, &fbo_);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

	Create(target, width, height);
}

RenderTexture::~RenderTexture() {
	glDeleteFramebuffers(1, &fbo_);
	glDeleteRenderbuffers(1, &depthBuffer_);
	glDeleteTextures(1, &targetTexture_);
}

void RenderTexture::Use() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
}

GLuint RenderTexture::GetTexture() const {
	return targetTexture_;
}

void RenderTexture::Create(RenderTarget target, GLint width, GLint height) {
	if (target == RenderTexture2D) {
		CreateTexture2D(width, height);
	}
	else {
		CreateDepthTexture(width, height);
	}
}

void RenderTexture::CreateTexture2D(GLint width, GLint height) {
	glGenTextures(1, &targetTexture_);
	glBindTexture(GL_TEXTURE_2D, targetTexture_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glGenRenderbuffers(1, &depthBuffer_);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, targetTexture_, 0);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(COUNT_OF(buffers), buffers);
}

void RenderTexture::CreateDepthTexture(GLint width, GLint height) {
	glGenTextures(1, &targetTexture_);
	glBindTexture(GL_TEXTURE_2D, targetTexture_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, targetTexture_, 0);

	GLenum buffers[] = { GL_NONE };
	glDrawBuffers(COUNT_OF(buffers), buffers);
}

RandomTexture::RandomTexture() {
	textureID_ = 0;
}

RandomTexture::~RandomTexture() {
}

void RandomTexture::Destroy() {
	if (textureID_ != 0) {
		glDeleteTextures(1, &textureID_);
		textureID_ = 0;
	}
}

bool RandomTexture::Load(unsigned size) {
	GLuint textureID = LoadRandomTexture(size);
	if (textureID != 0) {
		Destroy();
		textureID_ = textureID;
		return true;
	}

	return false;
}

void RandomTexture::Use() {
	Assert(textureID_ != 0, "invalid texture.");
	glBindTexture(GL_TEXTURE_1D, textureID_);
}

GLuint RandomTexture::LoadRandomTexture(unsigned size) {
	glm::vec3* data = new glm::vec3[size];
	for (unsigned i = 0; i < size; ++i) {
		data[i].x = (float)rand() / RAND_MAX;
		data[i].y = (float)rand() / RAND_MAX;
		data[i].z = (float)rand() / RAND_MAX;
	}

	GLuint textureID = 0;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_1D, textureID);

	// It is a 1D texture with the GL_RGB internal format and floating point data type. 
	// This means that every element is a vector of 3 floating point values.
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, size, 0, GL_RGB, GL_FLOAT, data);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// This allows us to use any texture coordinate to access the texture. 
	// If the texture coordinate is more than 1.0 it is simply wrapped around so it always retrieves a valid value.
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	delete[] data;

	return textureID;
}
