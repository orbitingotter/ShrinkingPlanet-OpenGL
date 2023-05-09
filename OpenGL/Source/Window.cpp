#include "pch.h"
#include "Window.h"


Window::Window(const std::string& title, int width, int height, bool vSync, bool fullScreen)
	: mTitle(title), mWidth(width), mHeight(height), mVSync(vSync),
	mPosX(0), mPosY(0),
	mMouseX(0.0f), mMouseY(0.0f), mLastMouseX(0.0f), mLastMouseY(0.0f),
	mDeltaMouseX(0.0f), mDeltaMouseY(0.0f), mFirstMouse(true)
{
	if (!glfwInit())
	{
		std::cout << "ERROR : GLFW was not initlialized" << std::endl;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	mWindow = glfwCreateWindow(width, height, title.c_str(), fullScreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (!mWindow)
	{
		glfwTerminate();
		std::cout << "ERROR : GLFW window could not be created" << std::endl;
		return;
	}

	glfwMakeContextCurrent(mWindow);
	glfwSwapInterval(mVSync);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return;
	}

	fprintf(stdout, "GLEW : Using GLEW %s\n", glewGetString(GLEW_VERSION));
	std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;

	// callbacks
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetCursorPosCallback(mWindow, [](GLFWwindow* glfwWindow, double xpos, double ypos)
		{
			Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			window->mMouseX = (float)xpos;
			window->mMouseY = (float)ypos;

			if (window->mFirstMouse)
			{
				window->mLastMouseX = (float)xpos;
				window->mLastMouseY = (float)ypos;
				window->mFirstMouse = false;
			}
			window->mDeltaMouseX = window->mMouseX -  window->mLastMouseX;
			window->mDeltaMouseY = -window->mMouseY + window->mLastMouseY;

			window->mLastMouseX = (float)xpos;
			window->mLastMouseY = (float)ypos;
		});

	glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* glfwWindow, int width, int height)
		{
			Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			window->mWidth = width;
			window->mHeight = height;
		});

	glfwSetWindowPosCallback(mWindow, [](GLFWwindow* glfwWindow, int xpos, int ypos)
		{
			Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			window->mPosX = xpos;
			window->mPosY = ypos;
		}
	);


}

Window::~Window()
{
	glfwTerminate();
}

bool Window::IsRunning() const
{
	return !glfwWindowShouldClose(mWindow);
}

void Window::Close() const
{
	glfwSetWindowShouldClose(mWindow, 1);
}

bool Window::IsVSync() const
{
	return mVSync;
}

void Window::SetVSync(bool vsync)
{
	mVSync = vsync;
	glfwSwapInterval(mVSync);
}

const std::string& Window::GetTitle() const
{
	return mTitle;
}

void Window::SetTitle(const std::string& title)
{
	mTitle = title;
	glfwSetWindowTitle(mWindow, title.c_str());
}

void Window::SetSize(int x, int y)
{
	mWidth = x;
	mHeight = y;
	glfwSetWindowSize(mWindow, x, y);
}

int Window::GetWidth() const
{
	return mWidth;
}

int Window::GetHeight() const
{
	return mHeight;
}

float Window::GetAspectRatio() const
{
	return ((float)GetWidth() / (float)GetHeight());
}

float Window::GetCurrentTime() const
{
	return glfwGetTime();
}

std::pair<int, int> Window::GetWindowPos()
{
	return { mPosX, mPosY };
}

GLFWwindow* Window::Get() const
{
	return mWindow;
}

bool Window::IsKeyPressed(int keycode)
{
	return glfwGetKey(mWindow, keycode) == GLFW_PRESS;
}

bool Window::IsMousePressed(int keycode)
{
	return glfwGetMouseButton(mWindow, keycode) == GLFW_PRESS;
}

std::pair<float, float> Window::GetMousePos()
{
	return { mMouseX, mMouseY };
}

std::pair<float, float> Window::GetMouseOffset()
{
	return { mDeltaMouseX, mDeltaMouseY };
}

void Window::OnUpdate()
{
	// reset delta as callback doesnot run when no changes
	mDeltaMouseX = 0.0f;
	mDeltaMouseY = 0.0f;

	glfwSwapBuffers(mWindow);
	glfwPollEvents();
}