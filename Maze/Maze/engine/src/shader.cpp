#include "debug.h"
#include "loader.h"
#include "shader.h"
#include "utilities.h"

static GLenum shaderTypeMap[] = {
	GL_VERTEX_SHADER,
	GL_GEOMETRY_SHADER,
	GL_FRAGMENT_SHADER,
};

static const char* shaderNameMap[] = {
	"VertexShader",
	"GeometryShader",
	"FragmentShader",
};

GLuint Shader::blockCount = 1;

Shader::Shader() {
	program_ = glCreateProgram();
	std::fill(shaderObjs_, shaderObjs_ + ShaderTypeCount, 0);
}

Shader::~Shader() {
	glDeleteProgram(program_);
	for (int i = 0; i < ShaderTypeCount; ++i) {
		if (shaderObjs_[i] != 0) {
			glDeleteShader(shaderObjs_[i]);
		}
	}
}

bool Shader::Load(ShaderType shaderType, const std::string& path) {
	std::string source;
	if (!TextLoader::Load(path, source)) {
		return false;
	}

	return LoadShader(shaderType, source.c_str());
}

bool Shader::Link() {
	for (int i = 0; i < ShaderTypeCount; ++i) {
		if (i != ShaderTypeGeometry) {
			Assert(shaderObjs_[i], Utility::Format("invalid shader, type = %d.", i));
		}
	}

	return LinkShader();
}

bool Shader::Use() {
	glUseProgram(program_);
	return true;
}

bool Shader::GetErrorMessage(GLuint shaderObj, std::string& answer) {
	if (shaderObj == 0) {
		answer = "invalid shader id";
		return false;
	}

	GLint length = 0, writen = 0;
	glGetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &length);
	if (length > 1) {
		char* log = new char[length];
		glGetShaderInfoLog(shaderObj, length, &writen, log);
		answer = log;
		delete[] log;
		return true;
	}

	return false;
}

void Shader::AddAllBlocks() {
	blocks_.clear();

	GLsizei nameWritten;
	GLint blockCount, uniformCount, blockNameLength, uniformNameLength, dataSize;
	GLint uniformType, uniformSize, uniformOffset, uniformMatrixStride, uniformArrayStride;

	GLuint buffer;
	std::string blockName, uniformName;

	glGetProgramiv(program_, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);
	for (GLint i = 0; i < blockCount; ++i) {
		glGetActiveUniformBlockiv(program_, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &blockNameLength);

		blockName.resize(blockNameLength);
		glGetActiveUniformBlockName(program_, i, blockNameLength, &nameWritten, &blockName[0]);
		blockName.resize(nameWritten);

		if (blocks_.contains(blockName)) {
			glUniformBlockBinding(program_, i, blocks_[blockName]->binding);
			continue;
		}

		UniformBlock* block = blocks_[blockName];

		glGetActiveUniformBlockiv(program_, i, GL_UNIFORM_BLOCK_DATA_SIZE, &dataSize);

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, buffer);
		glBufferData(GL_UNIFORM_BUFFER, dataSize, NULL, GL_DYNAMIC_DRAW);

		glUniformBlockBinding(program_, i, blockCount);
		glBindBufferRange(GL_UNIFORM_BUFFER, blockCount, buffer, 0, dataSize);

		glGetActiveUniformBlockiv(program_, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount);

		GLuint* indices = new GLuint[uniformCount];
		glGetActiveUniformBlockiv(program_, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (GLint*)indices);

		glGetProgramiv(program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameLength);
		uniformName.resize(uniformNameLength);

		for (GLint j = 0; j < uniformCount; ++j) {
			glGetActiveUniformName(program_, indices[j], uniformNameLength, &nameWritten, &uniformName[0]);
			uniformName.resize(nameWritten);

			Uniform* uniform = block->uniforms[uniformName];

			// for uniform Matrices { mat4 m };
			// uniformSize = 1, uniformMatrixStride = 16, uniformArrayStride = 0.
			// for uniform Matrices { mat4 m[2] }:
			// uniformSize = 2, uniformMatrixStride = 16, uniformArrayStride = 64.
			// for uniform Matrices { float m[4] }:
			// uniformSize = 4, uniformMatrixStride = 0, uniformArrayStride = 4.
			glGetActiveUniformsiv(program_, 1, &indices[j], GL_UNIFORM_TYPE, &uniformType);
			glGetActiveUniformsiv(program_, 1, &indices[j], GL_UNIFORM_SIZE, &uniformSize);
			glGetActiveUniformsiv(program_, 1, &indices[j], GL_UNIFORM_OFFSET, &uniformOffset);
			glGetActiveUniformsiv(program_, 1, &indices[j], GL_UNIFORM_MATRIX_STRIDE, &uniformMatrixStride);
			glGetActiveUniformsiv(program_, 1, &indices[j], GL_UNIFORM_ARRAY_STRIDE, &uniformArrayStride);

			uniform->offset = uniformOffset;
			uniform->type = uniformType;
			uniform->size = GetUniformSize(uniformType, uniformSize, uniformOffset, uniformMatrixStride, uniformArrayStride);
			uniform->stride = uniformArrayStride;
		}

		block->size = dataSize;
		block->binding = blockCount++;
		block->buffer = buffer;
	}
}

void Shader::AddAllUniforms() {
	uniforms_.clear();

	GLenum type;
	GLuint location = 0;
	GLint size, count, maxLength, length, stride;

	glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &count);
	glGetProgramiv(program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

	char* name = new char[maxLength];
	for (int i = 0; i < count; ++i) {
		glGetActiveUniform(program_, i, maxLength, &length, &size, &type, name);
		
		location = glGetUniformLocation(program_, name);
		glGetActiveUniformsiv(program_, 1, &location, GL_UNIFORM_ARRAY_STRIDE, &stride);
		// -1 indicates that is not an active uniform, although it may be present in a
		// uniform block
		if (location != GL_INVALID_INDEX) {
			Uniform* uniform = uniforms_[name];
			uniform->type = type;
			uniform->location = location;
			uniform->size = size;
			uniform->stride = stride;
		}
	}

	delete[] name;
}

bool Shader::LinkShader() {
	glLinkProgram(program_);
	
	GLint status = GL_FALSE;
	glGetProgramiv(program_, GL_LINK_STATUS, &status);

	if (status != GL_TRUE) {
		Debug::LogError("failed to link shader.");
		return false;
	}

	AddAllUniforms();
	AddAllBlocks();
	return true;
}

bool Shader::LoadShader(ShaderType shaderType, const char* source) {
	GLuint shaderObj = glCreateShader(shaderTypeMap[shaderType]);

	glShaderSource(shaderObj, 1, &source, nullptr);
	glCompileShader(shaderObj);

	glAttachShader(program_, shaderObj);

	std::string message;
	if (!GetErrorMessage(shaderObj, message)) {
		if (shaderObjs_[shaderType] != 0) {
			glDeleteShader(shaderObjs_[shaderType]);
		}
		shaderObjs_[shaderType] = shaderObj;
		return true;
	}

	Assert(false, shaderNameMap[shaderType] + std::string(" ") + message);
	return false;
}

void Shader::SetUniform(const std::string& name, int value) {
	Assert(uniforms_.contains(name), "invalid uniform " + name + ".");
	Uniform* u = uniforms_[name];
	glProgramUniform1i(program_, u->location, value);
}

void Shader::SetUniform(const std::string& name, float value) {
	Assert(uniforms_.contains(name), "invalid uniform " + name + ".");
	Uniform* u = uniforms_[name];
	glProgramUniform1f(program_, u->location, value);
}

void Shader::SetUniform(const std::string& name, const void* value) {
	Assert(uniforms_.contains(name), "invalid uniform " + name + ".");
	Uniform* u = uniforms_[name];
	switch (u->type) {

		// Floats
	case GL_FLOAT:
		glProgramUniform1fv(program_, u->location, u->size, (const GLfloat *)value);
		break;
	case GL_FLOAT_VEC2:
		glProgramUniform2fv(program_, u->location, u->size, (const GLfloat *)value);
		break;
	case GL_FLOAT_VEC3:
		glProgramUniform3fv(program_, u->location, u->size, (const GLfloat *)value);
		break;
	case GL_FLOAT_VEC4:
		glProgramUniform4fv(program_, u->location, u->size, (const GLfloat *)value);
		break;

		// Doubles
	case GL_DOUBLE:
		glProgramUniform1dv(program_, u->location, u->size, (const GLdouble *)value);
		break;
	case GL_DOUBLE_VEC2:
		glProgramUniform2dv(program_, u->location, u->size, (const GLdouble *)value);
		break;
	case GL_DOUBLE_VEC3:
		glProgramUniform3dv(program_, u->location, u->size, (const GLdouble *)value);
		break;
	case GL_DOUBLE_VEC4:
		glProgramUniform4dv(program_, u->location, u->size, (const GLdouble *)value);
		break;

		// Samplers, Ints and Bools
	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_MULTISAMPLE:
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_BUFFER:
	case GL_SAMPLER_2D_RECT:
	case GL_SAMPLER_2D_RECT_SHADOW:
	case GL_INT_SAMPLER_1D:
	case GL_INT_SAMPLER_2D:
	case GL_INT_SAMPLER_3D:
	case GL_INT_SAMPLER_CUBE:
	case GL_INT_SAMPLER_1D_ARRAY:
	case GL_INT_SAMPLER_2D_ARRAY:
	case GL_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_INT_SAMPLER_BUFFER:
	case GL_INT_SAMPLER_2D_RECT:
	case GL_UNSIGNED_INT_SAMPLER_1D:
	case GL_UNSIGNED_INT_SAMPLER_2D:
	case GL_UNSIGNED_INT_SAMPLER_3D:
	case GL_UNSIGNED_INT_SAMPLER_CUBE:
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_BUFFER:
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
	case GL_BOOL:
	case GL_INT:
		glProgramUniform1iv(program_, u->location, u->size, (const GLint *)value);
		break;
	case GL_BOOL_VEC2:
	case GL_INT_VEC2:
		glProgramUniform2iv(program_, u->location, u->size, (const GLint *)value);
		break;
	case GL_BOOL_VEC3:
	case GL_INT_VEC3:
		glProgramUniform3iv(program_, u->location, u->size, (const GLint *)value);
		break;
	case GL_BOOL_VEC4:
	case GL_INT_VEC4:
		glProgramUniform4iv(program_, u->location, u->size, (const GLint *)value);
		break;

		// Unsigned ints
	case GL_UNSIGNED_INT:
		glProgramUniform1uiv(program_, u->location, u->size, (const GLuint *)value);
		break;
	case GL_UNSIGNED_INT_VEC2:
		glProgramUniform2uiv(program_, u->location, u->size, (const GLuint *)value);
		break;
	case GL_UNSIGNED_INT_VEC3:
		glProgramUniform3uiv(program_, u->location, u->size, (const GLuint *)value);
		break;
	case GL_UNSIGNED_INT_VEC4:
		glProgramUniform4uiv(program_, u->location, u->size, (const GLuint *)value);
		break;

		// Float Matrices
	case GL_FLOAT_MAT2:
		glProgramUniformMatrix2fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT3:
		glProgramUniformMatrix3fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT4:
		glProgramUniformMatrix4fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT2x3:
		glProgramUniformMatrix2x3fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT2x4:
		glProgramUniformMatrix2x4fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT3x2:
		glProgramUniformMatrix3x2fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT3x4:
		glProgramUniformMatrix3x4fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT4x2:
		glProgramUniformMatrix4x2fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT4x3:
		glProgramUniformMatrix4x3fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;

		// Double Matrices
	case GL_DOUBLE_MAT2:
		glProgramUniformMatrix2dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT3:
		glProgramUniformMatrix3dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT4:
		glProgramUniformMatrix4dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT2x3:
		glProgramUniformMatrix2x3dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT2x4:
		glProgramUniformMatrix2x4dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT3x2:
		glProgramUniformMatrix3x2dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT3x4:
		glProgramUniformMatrix3x4dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT4x2:
		glProgramUniformMatrix4x2dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT4x3:
		glProgramUniformMatrix4x3dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	}
}

void Shader::SetBlock(const std::string& name, const void* value) {
	Assert(blocks_.contains(name), "invalid block name " + name + ".");
	UniformBlock* block = blocks_[name];
	glBindBuffer(GL_UNIFORM_BUFFER, block->buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, block->size, value);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Shader::SetBlockUniform(const std::string& blockName, const std::string& uniformName, const void* value) {
	Assert(blocks_.contains(blockName), "invalid block name " + blockName + ".");
	UniformBlock* block = blocks_[blockName];

	std::string composedUniformName = blockName + "." + uniformName;
	std::string finalUniformName;

	if (block->uniforms.contains(uniformName)) {
		finalUniformName = uniformName;
	}
	else if(block->uniforms.contains(composedUniformName)) {
		finalUniformName = composedUniformName;
	}

	Assert(!finalUniformName.empty(), "invalid uniform name " + uniformName + ".");
	Uniform* uniform = block->uniforms[finalUniformName];
	glBindBuffer(GL_UNIFORM_BUFFER, block->buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, uniform->offset, uniform->size, value);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Shader::SetBlockUniformArrayElement(const std::string& blockName, const std::string& uniformName, GLint index, const void* value) {
	Assert(blocks_.contains(blockName), "invalid block name " + blockName + ".");
	UniformBlock* block = blocks_[blockName];
	Assert(block->uniforms.contains(uniformName), "invalid uniform name " + uniformName + ".");
	Uniform* uniform = block->uniforms[uniformName];

	glBindBuffer(GL_UNIFORM_BUFFER, block->buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, uniform->offset + uniform->stride * index, uniform->stride, value);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

GLuint Shader::GetUniformSize(GLint uniformType, GLint uniformSize, 
	GLint uniformOffset, GLint uniformMatrixStride, GLint uniformArrayStride) {
	if (uniformArrayStride > 0) {
		return uniformSize * uniformArrayStride;
	}

	if (uniformMatrixStride > 0) {
		switch (uniformType) {
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
		case GL_DOUBLE_MAT2:
		case GL_DOUBLE_MAT2x3:
		case GL_DOUBLE_MAT2x4:
			return 2 * uniformMatrixStride;

		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
		case GL_DOUBLE_MAT3:
		case GL_DOUBLE_MAT3x2:
		case GL_DOUBLE_MAT3x4:
			return 3 * uniformMatrixStride;

		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
		case GL_DOUBLE_MAT4:
		case GL_DOUBLE_MAT4x2:
		case GL_DOUBLE_MAT4x3:
			return 4 * uniformMatrixStride;

		default:
			Debug::LogError("invalid uniformType " + std::to_string(uniformType) + ".");
			return 0;
		}
	}

	return GetSizeOfType(uniformType);
}

GLuint Shader::GetSizeOfType(GLint type) {
	switch (type) {
	case GL_FLOAT:
		return sizeof(float);

	case GL_FLOAT_VEC2:
		return sizeof(float) * 2;

	case GL_FLOAT_VEC3:
		return sizeof(float) * 3;

	case GL_FLOAT_VEC4:
		return sizeof(float) * 4;

		// Doubles
	case GL_DOUBLE:
		return sizeof(double);

	case GL_DOUBLE_VEC2:
		return sizeof(double) * 2;

	case GL_DOUBLE_VEC3:
		return sizeof(double) * 3;

	case GL_DOUBLE_VEC4:
		return sizeof(double) * 4;

		// Samplers, Ints and Bools
	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_MULTISAMPLE:
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_BUFFER:
	case GL_SAMPLER_2D_RECT:
	case GL_SAMPLER_2D_RECT_SHADOW:
	case GL_INT_SAMPLER_1D:
	case GL_INT_SAMPLER_2D:
	case GL_INT_SAMPLER_3D:
	case GL_INT_SAMPLER_CUBE:
	case GL_INT_SAMPLER_1D_ARRAY:
	case GL_INT_SAMPLER_2D_ARRAY:
	case GL_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_INT_SAMPLER_BUFFER:
	case GL_INT_SAMPLER_2D_RECT:
	case GL_UNSIGNED_INT_SAMPLER_1D:
	case GL_UNSIGNED_INT_SAMPLER_2D:
	case GL_UNSIGNED_INT_SAMPLER_3D:
	case GL_UNSIGNED_INT_SAMPLER_CUBE:
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_BUFFER:
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
	case GL_BOOL:
	case GL_INT:
		return sizeof(int);

	case GL_BOOL_VEC2:
	case GL_INT_VEC2:
		return sizeof(int) * 2;

	case GL_BOOL_VEC3:
	case GL_INT_VEC3:
		return sizeof(int) * 3;

	case GL_BOOL_VEC4:
	case GL_INT_VEC4:
		return sizeof(int) * 4;

		// Unsigned ints
	case GL_UNSIGNED_INT:
		return sizeof(unsigned int);

	case GL_UNSIGNED_INT_VEC2:
		return sizeof(unsigned int) * 2;

	case GL_UNSIGNED_INT_VEC3:
		return sizeof(unsigned int) * 3;

	case GL_UNSIGNED_INT_VEC4:
		return sizeof(unsigned int) * 4;

		// Float Matrices
	case GL_FLOAT_MAT2:
		return sizeof(float) * 4;

	case GL_FLOAT_MAT3:
		return sizeof(float) * 9;

	case GL_FLOAT_MAT4:
		return sizeof(float) * 16;

	case GL_FLOAT_MAT2x3:
		return sizeof(float) * 6;

	case GL_FLOAT_MAT2x4:
		return sizeof(float) * 8;

	case GL_FLOAT_MAT3x2:
		return sizeof(float) * 6;

	case GL_FLOAT_MAT3x4:
		return sizeof(float) * 12;

	case GL_FLOAT_MAT4x2:
		return sizeof(float) * 8;

	case GL_FLOAT_MAT4x3:
		return sizeof(float) * 12;

		// Double Matrices
	case GL_DOUBLE_MAT2:
		return sizeof(double) * 4;

	case GL_DOUBLE_MAT3:
		return sizeof(double) * 9;

	case GL_DOUBLE_MAT4:
		return sizeof(double) * 16;

	case GL_DOUBLE_MAT2x3:
		return sizeof(double) * 6;

	case GL_DOUBLE_MAT2x4:
		return sizeof(double) * 8;

	case GL_DOUBLE_MAT3x2:
		return sizeof(double) * 6;

	case GL_DOUBLE_MAT3x4:
		return sizeof(double) * 12;

	case GL_DOUBLE_MAT4x2:
		return sizeof(double) * 8;

	case GL_DOUBLE_MAT4x3:
		return sizeof(double) * 12;

	default:
		Debug::LogError("invalid uniform type " + std::to_string(type) + ".");
		return 0;
	}

	return 0;
}

Shader::UniformBlock::UniformBlock() : buffer(0) {
}

Shader::UniformBlock::~UniformBlock() {
	if (buffer != 0) {
		glDeleteBuffers(1, &buffer);
	}
}
