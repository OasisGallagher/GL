#include "debug.h"
#include "reader.h"
#include "shader.h"
#include "utilities.h"

static GLenum shaderTypeMap[] = {
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
};

static const char* shaderNameMap[] = {
	"VertexShader",
	"FragmentShader",
};

Shader::Shader() {
	program_ = glCreateProgram();
	std::fill(shaderObjs_, shaderObjs_ + ShaderTypeCount, 0);
}

Shader::~Shader() {
	glDeleteProgram(program_);
	DeleteAllShaders();
}

bool Shader::Load(ShaderType shaderType, const std::string& path) {
	std::string source;
	if (!Reader::ReadAll(path, source)) {
		return false;
	}

	return LoadShader(shaderType, source.c_str());
}

bool Shader::Link() {
	for (int i = 0; i < ShaderTypeCount; ++i) {
		Assert(shaderObjs_[i], Utility::Format("invalid shader, type = %d.", i));
	}

	return LinkShader();
}

bool Shader::Use() {
	glUseProgram(program_);
	return true;
}

void Shader::DeleteAllShaders() {
	for (int i = 0; i < ShaderTypeCount; ++i) {
		if (shaderObjs_[i] != 0) {
			glDeleteShader(shaderObjs_[i]);
		}
	}
}

bool Shader::GetInfoLog(GLuint shaderObj, std::string& answer) {
	if (shaderObj == 0) {
		answer = "invalid shader id";
		return false;
	}

	GLint length = 0, writen = 0;
	glGetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &length);
	if (length > 0) {
		char* log = new char[length];
		glGetShaderInfoLog(shaderObj, length, &writen, log);
		answer = log;
		delete[] log;

		return false;
	}

	return true;
}

void Shader::AddAllUniforms() {
	uniforms_.clear();

	GLenum type;
	GLuint location = 0;
	GLint count = 0, maxLength = 0, length = 0;
	GLint size = 0, arrayStride = 0;

	glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &count);
	glGetProgramiv(program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

	char* name = new char[maxLength];
	for (int i = 0; i < count; ++i) {
		glGetActiveUniform(program_, i, maxLength, &length, &size, &type, name);
		location = glGetUniformLocation(program_, name);
		glGetActiveUniformsiv(program_, 1, &location, GL_UNIFORM_ARRAY_STRIDE, &arrayStride);
		// -1 indicates that is not an active uniform, although it may be present in a
		// uniform block
		if (location != -1) {
			Uniform* uniform = new Uniform;
			uniform->type = type;
			uniform->location = location;
			uniform->size = size;
			uniforms_.insert(std::make_pair(name, uniform));
		}
	}

	delete[] name;
}

bool Shader::LinkShader() {
	glLinkProgram(program_);

	AddAllUniforms();

	GLint status = GL_FALSE;
	glGetProgramiv(program_, GL_LINK_STATUS, &status);
	return status == GL_TRUE;
}

bool Shader::LoadShader(ShaderType shaderType, const char* source) {
	GLuint shaderObj = glCreateShader(shaderTypeMap[shaderType]);

	glShaderSource(shaderObj, 1, &source, nullptr);
	glCompileShader(shaderObj);

	glAttachShader(program_, shaderObj);

	std::string message;
	if (GetInfoLog(shaderObj, message)) {
		if (shaderObjs_[shaderType] != 0) {
			glDeleteShader(shaderObjs_[shaderType]);
		}
		shaderObjs_[shaderType] = shaderObj;
		return true;
	}

	Debug::LogError(shaderNameMap[shaderType] + std::string(" ") + message);
	return false;
}

void Shader::SetUniform(const std::string& name, int value) {
	Assert(uniforms_.count(name) != 0, "invalid uniform " + name + ".");
	Uniform* u = uniforms_[name];
	glProgramUniform1i(program_, u->location, value);
}

void Shader::SetUniform(const std::string& name, float value) {
	Assert(uniforms_.count(name) != 0, "invalid uniform " + name + ".");
	Uniform* u = uniforms_[name];
	glProgramUniform1f(program_, u->location, value);
}

void Shader::SetUniform(const std::string& name, void* value) {
	Assert(uniforms_.count(name) != 0, "invalid uniform " + name + ".");
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
