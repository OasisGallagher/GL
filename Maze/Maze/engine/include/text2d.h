#pragma once
#include <string>
#include <gl/glew.h>

class Shader;
class Texture;

class Text2D {
public:
	Text2D();
	~Text2D();

public:
	bool Load(const std::string& path);
	void Print(const std::string& text, int x, int y, int size);

private:
	GLuint vao_;
	GLuint vbo_[2];

	Shader* shader_;
	Texture* texture_;
};