#include "pch.h"
#include "Camera.h"


Camera::Camera()
	:mActivated(false)
{
	mPos = glm::vec3(0.0f, 10.0f, -20.0f);
	mFront = glm::vec3(0.0f, 0.0f, 1.0f);
	mWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	mUpDir = glm::vec3(0.0f, 1.0f, 0.0f);

	mSpeed = 0.0f;
	mSpeedSensitivity = 12.0f;
	mMouseSensitivity = 0.2f;

	mYaw = -90.0f;
	mRoll = 0.0f;
	mPitch = 0.0f;

}

Camera::~Camera()
{
}

void Camera::OnUpdate(Window& window, float delta)
{
	// prev status stops multiple boolean toggles
	static bool sPrevPressed = false;
	bool pressed = window.IsKeyPressed(GLFW_KEY_C);
	if (pressed && !sPrevPressed)
	{
		if (mActivated)
		{
			mActivated = false;
			glfwSetInputMode(window.Get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			mActivated = true;
			glfwSetInputMode(window.Get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	sPrevPressed = pressed;

	if (mActivated)
	{
		mSpeed = delta * mSpeedSensitivity;

		// KEYBOARD
		// front - back
		if (window.IsKeyPressed(GLFW_KEY_W))
			mPos += mSpeed * mFront;
		if (window.IsKeyPressed(GLFW_KEY_S))
			mPos -= mSpeed * mFront;

		// left - right
		if (window.IsKeyPressed(GLFW_KEY_A))
			mPos += mSpeed * glm::cross(mUpDir, mFront);
		if (window.IsKeyPressed(GLFW_KEY_D))
			mPos -= mSpeed * glm::cross(mUpDir, mFront);

		// up - down
		if (window.IsKeyPressed(GLFW_KEY_SPACE))
			mPos += mSpeed * mWorldUp;
		if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
			mPos -= mSpeed * mWorldUp;

		// MOUSE

		float xOffset = window.GetMouseOffset().first;
		float yOffset = window.GetMouseOffset().second;

		mYaw += mMouseSensitivity * xOffset;
		mPitch += mMouseSensitivity * yOffset;

		if (mPitch > 89.9f)
			mPitch = 89.9f;
		if (mPitch < -89.9f)
			mPitch = -89.9f;

		glm::vec3 front;
		front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		front.y = sin(glm::radians(mPitch));
		front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		mFront = glm::normalize(front);

		// recalculate vectors
		glm::vec3 right = glm::normalize(glm::cross(mFront, mWorldUp));
		mUpDir = glm::normalize(glm::cross(right, mFront));
	}
}

void Camera::SetActive(bool status)
{
	mActivated = status;
}

bool Camera::IsActive()
{
	return mActivated;
}
