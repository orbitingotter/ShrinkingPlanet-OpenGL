#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window(const std::string& title, int width, int height, bool vSync = true, bool fullScreen = false);
	~Window();

	bool IsRunning() const;
	void Close() const;

	void OnUpdate();

	// getters and setters
	bool IsVSync() const;
	void SetVSync(bool vsync);

	const std::string& GetTitle() const;
	void SetTitle(const std::string& title);

	void SetSize(int x, int y);
	int GetWidth() const;
	int GetHeight() const;
	float GetAspectRatio() const;
	float GetCurrentTime() const;
	std::pair<int, int> GetWindowPos();

	GLFWwindow* Get() const;

	// input polling
	bool IsKeyPressed(int keycode);
	bool IsMousePressed(int keycode);
	std::pair<float, float> GetMousePos();
	std::pair<float, float > GetMouseOffset();

private:
	std::string mTitle;
	int mWidth, mHeight;
	GLFWwindow* mWindow;
	bool mVSync;
	int mPosX, mPosY;


	float mMouseX, mMouseY;				// current mouse position
	float mLastMouseX, mLastMouseY;		// last frame mouse position
	float mDeltaMouseX, mDeltaMouseY;	// offset mouse position
	bool mFirstMouse;					// runs only fist time callback gets called

};