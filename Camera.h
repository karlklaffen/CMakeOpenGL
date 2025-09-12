#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/string_cast.hpp>

class Camera {

public:
	Camera(glm::vec3 pos, float yaw, float pitch, float zoom, float near = 0.01f, float far = 100.0f, float aspect = 1.0f);

	glm::mat4 GetViewMat() const;
	glm::mat4 GetProjMat() const;
	glm::vec3 GetPos() const;
	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetForward() const;
	float GetYaw() const;
	float GetPitch() const;
	float GetFOVRad() const;
	float GetFOVDeg() const;
	float GetFar() const;
	float GetGravity() const;
	glm::vec3 GetFPSForward() const;
	glm::vec3 GetFPSRight() const;

	void SetPos(glm::vec3 v);
	void SetForward(glm::vec3 v);
	void SetYawPitch(float y, float p);
	void SetFOVDeg(float fov);
	void SetFOVRad(float fov);
	void SetAspect(float a);
	void SetFar(float f);
	void SetGravity(float g);

	void Move(glm::vec3 v);

	void UpdateView();
	void UpdateProj();

private:
	glm::vec3 Pos, Right, Up, Forward;
	float Yaw, Pitch;

	glm::mat4 ViewMat, ProjMat;

	float FOV, Aspect, Near, Far;

	float Gravity;
};