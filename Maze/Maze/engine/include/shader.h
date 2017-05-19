#pragma once
#include <map>
#include <string>
#include <gl/glew.h>

#include "ptr_table.h"

enum ShaderType {
	ShaderTypeVertex,
	ShaderTypeTessellationControl,
	ShaderTypeTessellationEvaluation,
	ShaderTypeGeometry,
	ShaderTypeFragment,
	ShaderTypeCount,
};

class Shader {
private:
	struct Uniform {
		GLenum type;
		union {
			GLuint offset;
			GLuint location;
		};

		GLuint size;
		GLuint stride;
	};

	typedef PtrTable<Uniform> Uniforms;

	struct UniformBlock {
		UniformBlock();
		~UniformBlock();

		GLuint size;
		GLuint buffer;
		GLuint binding;
		
		Uniforms uniforms;
	};

	typedef PtrTable<UniformBlock> UniformBlocks;

public:
	Shader();
	~Shader();

public:
	bool Load(ShaderType shaderType, const std::string& path);
	bool Link();
	bool Use();

public:
	GLuint GetProgram() const { return program_; }

	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, const void* value);

	void SetBlock(const std::string& name, const void* value);
	void SetBlockUniform(const std::string& blockName, const std::string& uniformName, const void* value);
	void SetBlockUniformArrayElement(const std::string& blockName, const std::string& uniformName, GLint index, const void* value);

private:
	bool LinkShader();
	bool LoadShader(ShaderType shaderType, const char* source);

	void AddAllBlocks();
	void AddAllUniforms();

	GLuint GetSizeOfType(GLint type);
	GLuint GetUniformSize(GLint uniformType, GLint uniformSize, GLint uniformOffset, GLint uniformMatrixStride, GLint uniformArrayStride);

	bool GetErrorMessage(GLuint shaderObj, std::string& answer);

private:
	Uniforms uniforms_;
	UniformBlocks blocks_;

	GLuint program_;
	GLuint shaderObjs_[ShaderTypeCount];

private:
	static GLuint blockCount;
};
