#pragma once
#include "loader.h"
#include "example.h"

class Example_NormalMapping : public Example {
public:
	Example_NormalMapping() {
		modelInfo_ = new ModelInfo;
		ModelLoader::Load("models/cylinder.obj", *modelInfo_);

		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;

		TBNParser::Parse(tangents, bitangents, *modelInfo_);

		//VBOIndexer::Index(*modelInfo_, tangents, bitangents, indices_, *modelInfo_, tangents, bitangents);

		normal_ = new Texture;
		normal_->Load("textures/normal.bmp");

		diffuse_ = new Texture;
		diffuse_->Load("textures/diffuse.dds");

		specular_ = new Texture;
		specular_->Load("textures/specular.dds");

		shader_->Load(ShaderTypeVertex, "shaders/normal_mapping.vert");
		shader_->Load(ShaderTypeFragment, "shaders/normal_mapping.frag");
		shader_->Link();

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		glGenBuffers(1, &vertexVbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVbo_);
		glBufferData(GL_ARRAY_BUFFER, modelInfo_->vertices.size() * sizeof(glm::vec3), &modelInfo_->vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &uvVbo_);
		glBindBuffer(GL_ARRAY_BUFFER, uvVbo_);
		glBufferData(GL_ARRAY_BUFFER, modelInfo_->uvs.size() * sizeof(glm::vec2), &modelInfo_->uvs[0], GL_STATIC_DRAW);

		glGenBuffers(1, &normalVbo_);
		glBindBuffer(GL_ARRAY_BUFFER, normalVbo_);
		glBufferData(GL_ARRAY_BUFFER, modelInfo_->normals.size() * sizeof(glm::vec3), &modelInfo_->normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &tangentVbo_);
		glBindBuffer(GL_ARRAY_BUFFER, tangentVbo_);
		glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);

		glGenBuffers(1, &bitangentVbo_);
		glBindBuffer(GL_ARRAY_BUFFER, bitangentVbo_);
		glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);

		//glGenBuffers(1, &indexVbo_);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo_);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned), &indices_[0], GL_STATIC_DRAW);

		camera_->Reset(glm::vec3(6, 4, 1), glm::vec3(0));
		const glm::mat4& proj = camera_->GetProjMatrix();
		const glm::mat4& view = camera_->GetViewMatrix();
		glm::mat4 mvp = proj * view;

		shader_->SetUniform("MVP", &mvp);

		glm::mat3 MV3x3 = glm::mat3(view);
		shader_->SetUniform("MV3x3", &MV3x3);

		shader_->SetUniform("V", &view);

		glActiveTexture(GL_TEXTURE0);
		diffuse_->Use();
		shader_->SetUniform("diffuseSampler", 0);

		glActiveTexture(GL_TEXTURE1);
		specular_->Use();
		//shader_->SetUniform("specularSampler", 1);

		glActiveTexture(GL_TEXTURE2);
		normal_->Use();
		shader_->SetUniform("normalSampler", 2);

		glm::vec3 lightPosition(0, 0, 4);// 3, 8, 5);
		shader_->SetUniform("lightPosition_worldspace", &lightPosition);
	}

	~Example_NormalMapping() {
		delete modelInfo_;
		delete normal_;
		delete diffuse_;
		delete specular_;

		glDeleteVertexArrays(1, &vao_);
		glDeleteVertexArrays(1, &uvVbo_);
		glDeleteBuffers(1, &vertexVbo_);
		glDeleteBuffers(1, &indexVbo_);
		glDeleteBuffers(1, &normalVbo_);
	}

public:
	virtual void Update(float deltaTime) {
		shader_->Use();

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVbo_);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvVbo_);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalVbo_);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, tangentVbo_);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, bitangentVbo_);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, modelInfo_->vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
	}

	virtual void GetEnvRequirement(AppEnv& env) {
		Example::GetEnvRequirement(env);
		env.depthTest = true;
		env.cullFace = true;
		env.blend = false;
	}

private:
	ModelInfo* modelInfo_;
	std::vector<unsigned> indices_;

	Texture* normal_;
	Texture* diffuse_, *specular_;

	GLuint vao_;
	GLuint vertexVbo_, uvVbo_, indexVbo_, normalVbo_;
	GLuint tangentVbo_, bitangentVbo_;
};
