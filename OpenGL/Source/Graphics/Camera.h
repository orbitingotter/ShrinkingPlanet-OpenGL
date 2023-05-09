#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Window.h"

class Camera
{
public:
	Camera();
	~Camera();
	void OnUpdate(Window& window, float delta);

	void SetActive(bool status);
	bool IsActive();

	inline glm::mat4 GetViewMatrix() { return glm::lookAt(mPos, mPos + mFront, mUpDir); }
	inline const glm::vec3& GetPosition() { return mPos; }
private:
	glm::vec3 mPos;			// position of camera
	glm::vec3 mFront;		// target of camera relative to position
	glm::vec3 mUpDir;		// up direction of camera
	glm::vec3 mWorldUp;		// permanently points to up ie y axis

	float mSpeed;
	float mSpeedSensitivity;

	float mMouseSensitivity;
	float mYaw, mRoll, mPitch;

	bool mActivated;
};