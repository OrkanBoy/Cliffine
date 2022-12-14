#pragma once
#include "Window.hpp"
#include "Device.hpp"
#include "Swapchain.hpp"
#include "Pipeline.hpp"
#include "Renderer.hpp"

#include <glm/glm.hpp>

namespace clf
{
	class App
	{
	public:
		App();
		~App() {}

		static App* Get() { return instance; }
		Window& GetWindow() { return window; }
		void Run();

	private:
		static App* instance;

		Window window {800, 600, "Cliffine Engine"};
		Device device { window };
		Swapchain swapchain { device };
		Pipeline pipeline { device, swapchain };
		Renderer renderer { pipeline, swapchain, device };
	};

	App* CreateApp();
}
