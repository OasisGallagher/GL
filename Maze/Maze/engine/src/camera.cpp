#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "mathf.h"
#include "camera.h"

Camera::Camera() {
	float fov = Mathf::PI / 6, aspect = 4.f / 3.f, near = 0.1f, far = 100.f;
	proj_ = glm::perspective(fov, aspect, near, far);

	fwd_.x = 0, fwd_.y = 0, fwd_.z = -1;
	right_.x = 1, right_.y = 0, right_.z = 0;
	up_.x = 0, up_.y = 1, up_.z = 0;
}

Camera::~Camera() {
}

void Camera::Reset(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
	pos_ = eye;
	fwd_ = glm::normalize(eye - center);
	right_ = glm::normalize(glm::cross(up, fwd_));
	up_ = glm::normalize(glm::cross(fwd_, right_));
}

void Camera::Pitch(float radian) {
	glm::quat q = glm::angleAxis(radian * Mathf::Rad2Deg, right_);
	up_ = q * up_;
	fwd_ = q * fwd_;
}

void Camera::Yaw(float radian) {
	glm::quat q = glm::angleAxis(radian * Mathf::Rad2Deg, up_);
	fwd_ = q * fwd_;
	right_ = q * right_;
}

void Camera::Roll(float radian) {
	glm::quat q = glm::angleAxis(radian * Mathf::Rad2Deg, fwd_);
	up_ = q * up_;
	right_ = q * right_;
}

void Camera::Walk(float length) {
	pos_ += fwd_ * length;
}

void Camera::Strafe(float length) {
	pos_ += right_ * length;
}

void Camera::Fly(float length) {
	pos_ += up_ * length;
}

const glm::mat4& Camera::GetProjMatrix() {
	return proj_;
}

const glm::mat4& Camera::GetViewMatrix() {
	up_ = glm::normalize(glm::cross(fwd_, right_));
	right_ = glm::normalize(glm::cross(up_, fwd_));

	view_[0][0] = right_.x;
	view_[1][0] = right_.y;
	view_[2][0] = right_.z;
	view_[3][0] = 0.f;

	view_[0][1] = up_.x;
	view_[1][1] = up_.y;
	view_[2][1] = up_.z;
	view_[3][1] = 0.f;

	view_[0][2] = fwd_.x;
	view_[1][2] = fwd_.y;
	view_[2][2] = fwd_.z;
	view_[3][2] = 0.f;

	view_[3][0] = -glm::dot(right_, pos_);
	view_[3][1] = -glm::dot(up_, pos_);
	view_[3][2] = -glm::dot(fwd_, pos_);
	view_[3][3] = 1.f;

	return view_;
}
