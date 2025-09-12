#include "Camera.h"

Camera::Camera(glm::vec3 pos, float yaw, float pitch, float fov, float near, float far, float aspect) : Pos(pos), Yaw(yaw), Pitch(pitch), FOV(fov), Aspect(aspect), Near(near), Far(far), Gravity(0.0f) {
	SetYawPitch(yaw, pitch);
	UpdateView();
	UpdateProj();
}

glm::mat4 Camera::GetViewMat() const {
	return ViewMat;
}

glm::mat4 Camera::GetProjMat() const {
	return ProjMat;
}

glm::vec3 Camera::GetPos() const {
	return Pos;
}

glm::vec3 Camera::GetRight() const {
	return Right;
}

glm::vec3 Camera::GetUp() const {
	return Up;
}

glm::vec3 Camera::GetForward() const {
	return Forward;
}

float Camera::GetYaw() const {
	return Yaw;
}

float Camera::GetPitch() const {
	return Pitch;
}

float Camera::GetFOVRad() const {
	return FOV;
}

float Camera::GetFOVDeg() const {
	return glm::degrees(FOV);
}

float Camera::GetFar() const {
	return Far;
}

float Camera::GetGravity() const {
	return Gravity;
}

glm::vec3 Camera::GetFPSForward() const {
	return glm::normalize(glm::vec3{ Forward.x, 0.0f, Forward.z });
}

glm::vec3 Camera::GetFPSRight() const {
	return glm::normalize(glm::vec3{ Right.x, 0.0f, Right.z });
}

void Camera::SetPos(glm::vec3 v) {
	Pos = v;
	UpdateView();
}

void Camera::SetForward(glm::vec3 v) {
	Forward = v;
	Right = glm::normalize(glm::cross(Forward, glm::vec3(0.0f, 1.0f, 0.0f)));
	Up = glm::cross(Right, Forward);
	UpdateView();
}

void Camera::SetYawPitch(float y, float p) {
	Yaw = y;
	Pitch = p;
	SetForward(glm::normalize(glm::vec3(cos(Yaw) * cos(Pitch), sin(Pitch), sin(Yaw) * cos(Pitch))));
}

void Camera::SetFOVDeg(float fov) {
	SetFOVRad(glm::radians(fov));
}

void Camera::SetFOVRad(float fov) {
	FOV = fov;
	UpdateProj();
}

void Camera::SetAspect(float a) {
	Aspect = a;
	UpdateProj();
}

void Camera::SetFar(float f) {
	Far = f;
	UpdateProj();
}

void Camera::SetGravity(float g) {
	Gravity = g;
}

void Camera::Move(glm::vec3 v) {
	Pos += v;
	UpdateView();
}

void Camera::UpdateView() {
	ViewMat = glm::lookAt(Pos, Pos + Forward, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::UpdateProj() {
	ProjMat = glm::perspective(FOV, Aspect, Near, Far);
}