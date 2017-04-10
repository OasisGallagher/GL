#pragma once
#include <glm/glm.hpp>

class Camera {
public:
	Camera();
	~Camera();

public:
	void Reset(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

public:
	void Pitch(float radian);
	void Yaw(float radian);
	void Roll(float radian);

	void Walk(float length);
	void Strafe(float length);
	void Fly(float length);

public:
	const glm::mat4& GetProjMatrix();
	const glm::mat4& GetViewMatrix();

private:
	glm::vec3 pos_;
	glm::vec3 fwd_, up_, right_;

	glm::mat4 proj_, view_;
};
