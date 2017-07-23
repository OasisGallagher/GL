#pragma once
#include <string>
#include <gl/glew.h>

class Shader;
class Texture2D;

class Text2D {
public:
	Text2D();
	~Text2D();

public:
	bool Load(const std::string& path);
	void SetColor(const glm::vec3& color);
	void Print(const std::string& text, int x, int y, int size);

private:
	GLuint vao_;
	GLuint vbo_[2];

	glm::vec3 color_;

	Shader* shader_;
	Texture2D* texture_;
};