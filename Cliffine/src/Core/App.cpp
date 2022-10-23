#include "App.hpp"
#include "Log.hpp"

namespace clf
{
	App* App::instance = nullptr;
	App::App()
	{
		instance = this;
	}

	void App::Run()
	{
		while (!window.IsClosed())
		{
			glfwPollEvents();
			renderer.DrawFrame();
		}

		device.WaitIdle();
	}
}