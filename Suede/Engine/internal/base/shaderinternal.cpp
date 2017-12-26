#include "variables.h"
#include "debug/debug.h"
#include "tools/string.h"
#include "meshinternal.h"
#include "shaderinternal.h"
#include "internal/base/glsldefines.h"
#include "internal/file/shadercompiler.h"

ShaderInternal::ShaderInternal() : ObjectInternal(ObjectTypeShader), textureUnitCount_(0){
	program_ = GL::CreateProgram();
	GL::GetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits_);

	std::fill(shaderObjs_, shaderObjs_ + ShaderStageCount, 0);
}

ShaderInternal::~ShaderInternal() {
	GL::DeleteProgram(program_);
	ClearIntermediateShaders();
}

bool ShaderInternal::Load(const std::string& path) {
	ShaderCompiler compiler;
	std::string sources[ShaderStageCount];
	if (!compiler.Compile(path + GLSL_POSTFIX, "", sources)) {
		return false;
	}

	for (int i = 0; i < ShaderStageCount; ++i) {
		if (!sources[i].empty() && !LoadSource((ShaderStage)i, sources[i].c_str())) {
			return false;
		}
	}

	path_ = path;
	ClearIntermediateShaders();
	
	UpdateVertexAttributes();
	UpdateFragmentAttributes();

	if (!Link()) {
		return false;
	}

	AddAllUniforms();
	return true;
}

void ShaderInternal::Bind(uint pass) {
	if (pass > passes_.size()) {
		Debug::LogError("index out of range.");
		return;
	}

	currentPass_ = pass;
	passes_[pass]->Bind();
}

void ShaderInternal::Unbind() {
	passes_[currentPass_]->Unbind();
}

bool ShaderInternal::SetProperty(const std::string& name, const void* data) {
	Uniform* uniform = nullptr;
	if (!uniforms_.get(name, uniform)) {
		return false;
	}

	SetUniform(uniform, data);
	return true;
}

void ShaderInternal::GetProperties(std::vector<ShaderProperty>& properties) {
	for (UniformContainer::iterator ite = uniforms_.begin(); ite != uniforms_.end(); ++ite) {
		ShaderProperty property{ ite->first, ite->second->type };
		properties.push_back(property);
	}
}

bool ShaderInternal::GetErrorMessage(GLuint shaderObj, std::string& answer) {
	if (shaderObj == 0) {
		answer = "invalid shader id";
		return false;
	}

	GLint length = 0, writen = 0;
	GL::GetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &length);
	if (length > 1) {
		answer.resize(length);
		GL::GetShaderInfoLog(shaderObj, length, &writen, &answer[0]);
		return true;
	}

	return false;
}

bool ShaderInternal::Link() {
	GL::LinkProgram(program_);
	
	GLint status = GL_FALSE;
	GL::GetProgramiv(program_, GL_LINK_STATUS, &status);

	if (status != GL_TRUE) {
		Debug::LogError("failed to link shader %s.", path_.c_str());
		return false;
	}

	GL::ValidateProgram(program_);
	GL::GetProgramiv(program_, GL_VALIDATE_STATUS, &status);
	if (status != GL_TRUE) {
		Debug::LogWarning("failed to validate shader %s.", path_.c_str());
		//return false;
	}

	return true;
}

void ShaderInternal::ClearIntermediateShaders() {
	for (int i = 0; i < ShaderStageCount; ++i) {
		if (shaderObjs_[i] != 0) {
			GL::DeleteShader(shaderObjs_[i]);
			shaderObjs_[i] = 0;
		}
	}
}

bool ShaderInternal::LoadSource(ShaderStage stage, const char* source) {
	GLuint shaderObj = GL::CreateShader(GetShaderDescription(stage).glShaderStage);

	GL::ShaderSource(shaderObj, 1, &source, nullptr);
	GL::CompileShader(shaderObj);

	GL::AttachShader(program_, shaderObj);
	
	std::string message;
	if (!GetErrorMessage(shaderObj, message)) {
		if (shaderObjs_[stage] != 0) {
			GL::DeleteShader(shaderObjs_[stage]);
		}
		shaderObjs_[stage] = shaderObj;
		return true;
	}

	Debug::LogError("%s %s.", GetShaderDescription(stage).name, message.c_str());
	return false;
}

void ShaderInternal::UpdateVertexAttributes() {
	GL::BindAttribLocation(program_, VertexAttribPosition, Variables::position);
	GL::BindAttribLocation(program_, VertexAttribTexCoord, Variables::texCoord);
	GL::BindAttribLocation(program_, VertexAttribNormal, Variables::normal);
	GL::BindAttribLocation(program_, VertexAttribTangent, Variables::tangent);
	GL::BindAttribLocation(program_, VertexAttribBoneIndexes, Variables::boneIndexes);
	GL::BindAttribLocation(program_, VertexAttribBoneWeights, Variables::boneWeights);

	GL::BindAttribLocation(program_, VertexAttribInstanceColor, Variables::instanceColor);
	GL::BindAttribLocation(program_, VertexAttribInstanceGeometry, Variables::instanceGeometry);
}

void ShaderInternal::UpdateFragmentAttributes() {
}

void ShaderInternal::AddAllUniforms() {
	uniforms_.clear();
	textureUnitCount_ = 0;

	GLenum type;
	GLuint location = 0;
	GLint size, count, maxLength, length;

	GL::GetProgramiv(program_, GL_ACTIVE_UNIFORMS, &count);
	GL::GetProgramiv(program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

	char* name = MEMORY_CREATE_ARRAY(char, maxLength);
	for (int i = 0; i < count; ++i) {
		GL::GetActiveUniform(program_, i, maxLength, &length, &size, &type, name);

		location = GL::GetUniformLocation(program_, name);

		// -1 indicates that is not an active uniform, although it may be present in a
		// uniform block.
		if (location == GL_INVALID_INDEX) {
			continue;
		}

		// TODO: uniform array.
		char* ptr = strrchr(name, '[');
		if (ptr != nullptr) {
			*ptr = 0;
		}

		AddUniform(name, type, location, size);
	}

	MEMORY_RELEASE_ARRAY(name);
}

void ShaderInternal::AddUniform(const char* name, GLenum type, GLuint location, GLint size) {
	Uniform* uniform = uniforms_[name];
	uniform->size = size;
	uniform->location = location;

	if (type == GL_INT) {
		uniform->type = ShaderPropertyTypeInt;
	}
	else if (type == GL_FLOAT) {
		uniform->type = ShaderPropertyTypeFloat;
	}
	else if (type == GL_FLOAT_MAT4) {
		uniform->type = (size == 1) ? ShaderPropertyTypeMatrix4 : ShaderPropertyTypeMatrix4Array;
	}
	else if (type == GL_BOOL) {
		uniform->type = ShaderPropertyTypeBool;
	}
	else if (type == GL_FLOAT_VEC3) {
		uniform->type = ShaderPropertyTypeVector3;
	}
	else if (type == GL_FLOAT_VEC4) {
		uniform->type = ShaderPropertyTypeVector4;
	}
	else if (IsSampler(type)) {
		if (textureUnitCount_ >= maxTextureUnits_) {
			Debug::LogError("too many textures.");
		}
		else {
			++textureUnitCount_;
			uniform->type = ShaderPropertyTypeTexture;
		}
	}
	else {
		Debug::LogError("undefined uniform type 0x%x.", type);
	}
}

void ShaderInternal::SetUniform(Uniform* uniform, const void* data) {
	switch (uniform->type) {
		case ShaderPropertyTypeInt:
		case ShaderPropertyTypeBool:
			GL::ProgramUniform1iv(program_, uniform->location, uniform->size, (const GLint *)data);
			break;
		case ShaderPropertyTypeFloat:
			GL::ProgramUniform1fv(program_, uniform->location, uniform->size, (const GLfloat *)data);
			break;
		case ShaderPropertyTypeMatrix4:
		case ShaderPropertyTypeMatrix4Array:
			GL::ProgramUniformMatrix4fv(program_, uniform->location, uniform->size, false, (const GLfloat *)data);
			break;
		case ShaderPropertyTypeVector3:
			GL::ProgramUniform3fv(program_, uniform->location, uniform->size, (const GLfloat *)data);
			break;
		case ShaderPropertyTypeVector4:
			GL::ProgramUniform4fv(program_, uniform->location, uniform->size, (const GLfloat *)data);
			break;
		default:
			Debug::LogError("unable to set uniform (type 0x%x).", uniform->type);
			break;
	}
}

bool ShaderInternal::IsSampler(int type) {
	switch (type) {
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
			return true;
	}

	return false;
}
