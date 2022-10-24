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
			//TODO: Draw fram while window resize
			glfwPollEvents();
			renderer.DrawFrame();
		}

		device.WaitIdle();
	}
}