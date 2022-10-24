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

		const static App& Get() { return *instance; }
		const Window& GetWindow() const { return window; }
		void Run();
		
		VertexLayout vertexLayout
		{
			AttributeType::Float2,
			AttributeType::Float3
		};

		vec<float> data
		{
			0.0f,-0.5f,  1.0f, 0.0f, 0.0f,
			0.5f, 0.5f,  0.0f, 0.5f, 0.5f,
		   -0.5f, 0.5f,  0.0f, 0.0f, 1.0f
		};
	private:
		static App* instance;

		Window window {800, 600, "Cliffine Engine"};
		Device device { window };
		Swapchain swapchain { device };
		Pipeline pipeline{ device, swapchain, vertexLayout };
		Model model { device, vertexLayout, (void*)data.data(), data.size() };
		Renderer renderer { pipeline, swapchain, device, model };
	};

	App* CreateApp();
}
