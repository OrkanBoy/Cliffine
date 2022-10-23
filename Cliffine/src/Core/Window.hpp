#pragma once
#include "Core.hpp"

namespace clf
{
	class Window
	{
	public:
		bool framebufferResized;

		Window(const i32& width, const i32& height, const str& name);
		~Window();
		
		const bool& IsClosed() const { return glfwWindowShouldClose(window); }
		GLFWwindow& GetNative() { return *window; }
		void InitSurface(VkInstance& instance, VkSurfaceKHR* surface);

	private:
		static void OnFrameBufferResize(GLFWwindow* window, int width, int height);

		GLFWwindow* window;
		i32 width, height;
		str name;
	};
}