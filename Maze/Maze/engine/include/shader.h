#pragma once
#include <map>
#include <string>
#include <gl/glew.h>

enum ShaderType {
	ShaderTypeVertex,
	ShaderTypeFragment,
	ShaderTypeCount,
};

class Shader {
private:
	struct Uniform {
		GLenum type;
		GLuint location;
		GLuint size;
	};

	typedef std::map<std::string, Uniform*> Uniforms;

public:
	Shader();
	~Shader();

public:
	bool Load(ShaderType shaderType, const std::string& path);
	bool Link();
	bool Use();

public:
	GLuint GetProgram() const;
	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, void* value);

private:
	bool LinkShader();
	bool LoadShader(ShaderType shaderType, const char* source);

	void AddAllUniforms();

	bool GetErrorMessage(GLuint shaderObj, std::string& answer);

private:
	Uniforms uniforms_;

	GLuint program_;
	GLuint shaderObjs_[ShaderTypeCount];
};

#include "shader.inl"
