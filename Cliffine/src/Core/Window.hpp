#pragma once
#include "Core.hpp"

namespace clf
{
	class Window
	{
	public:
		Window(const i32& width, const i32& height, const str& name);
		~Window();
		
		const bool& ShouldClose() { return glfwWindowShouldClose(window); }
		GLFWwindow& GetNative() { return *window; }
		void InitSurface(VkInstance& instance, VkSurfaceKHR* surface);

	private:
		GLFWwindow* window;
		i32 width, height;
		str name;
	};
}