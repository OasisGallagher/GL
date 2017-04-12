#include <glm/glm.hpp>

#include "app.h"
#include "text2d.h"
#include "shader.h"
#include "texture.h"
#include "utilities.h"

extern App app;

Text2D::Text2D() {
	shader_ = new Shader;
	texture_ = new Texture;

 	glGenVertexArrays(1, &vao_);
 	glBindVertexArray(vao_);
 
 	glGenBuffers(COUNT_OF(vbo_), vbo_);

	shader_->Load(ShaderTypeVertex, "shaders/text2d.vert");
	shader_->Load(ShaderTypeFragment, "shaders/text2d.frag");
	shader_->Link();
}

Text2D::~Text2D() {
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(COUNT_OF(vbo_), vbo_);

	delete shader_;
	delete texture_;
}

bool Text2D::Load(const std::string& path) {
	return texture_->Load(path);
}

void Text2D::Print(const std::string& text, int x, int y, int size) {
	const float uvOffset = 1 / 16.f;

	glm::vec2 halfWindowSize(200, 150);

	std::vector<glm::vec2> uvs;
	std::vector<glm::vec2> vertices;
	float fx = (float)x, fy = (float)y;

	for (unsigned i = 0; i < text.length(); ++i) {
		glm::vec2 vertex[4], uv[4];

		vertex[0].x = fx + i * size, vertex[0].y = fy + size;
		vertex[1].x = fx + (i + 1) * size, vertex[1].y = fy + size;
		vertex[2].x = fx + (i + 1) * size, vertex[2].y = fy;
		vertex[3].x = fx + i * size, vertex[3].y = fy;

		vertex[0] -= halfWindowSize, vertex[0] /= halfWindowSize;
		vertex[1] -= halfWindowSize, vertex[1] /= halfWindowSize;
		vertex[2] -= halfWindowSize, vertex[2] /= halfWindowSize;
		vertex[3] -= halfWindowSize, vertex[3] /= halfWindowSize;

		vertices.push_back(vertex[0]);
		vertices.push_back(vertex[3]);
		vertices.push_back(vertex[1]);

		vertices.push_back(vertex[2]);
		vertices.push_back(vertex[1]);
		vertices.push_back(vertex[3]);

		char c = text[i];
		float u = (c % 16) / 16.f;
		float v = (c / 16) / 16.f;
		
		uv[0].s = u, uv[0].t = v;
		uv[1].s = u + uvOffset, uv[1].t = v;
		uv[2].s = u + uvOffset, uv[2].t = v + uvOffset;
		uv[3].s = u, uv[3].t = v + uvOffset;

		uvs.push_back(uv[0]);
		uvs.push_back(uv[3]);
		uvs.push_back(uv[1]);

		uvs.push_back(uv[2]);
		uvs.push_back(uv[1]);
		uvs.push_back(uv[3]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	shader_->Use();
	glActiveTexture(GL_TEXTURE0);
	texture_->Use();
	shader_->SetUniform("sampler", 0);
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
