#include "pch.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// test scenes
#include "Tests/ShadowTest.h"
#include "Tests/GameApp.h"


int main()
{
	Sandbox* scene = new GameApp();
	bool restart = scene->restart;

	while (restart)
	{
		scene->Run();

		restart = scene->restart;
		delete scene;
		if(restart)
			scene = new GameApp();
	}


	return 0;
}