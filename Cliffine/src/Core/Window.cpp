#include "Window.hpp"

namespace clf
{
	Window::Window(const i32& width, const i32& height, const str& name)
		: width(width), height(height), name(name)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, OnFrameBufferResize);
	}

	Window::~Window()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Window::OnFrameBufferResize(GLFWwindow* window, int width, int height)
	{
		auto clfWindow = (Window*)glfwGetWindowUserPointer(window);
		clfWindow->framebufferResized = true;
	}

	void Window::InitSurface(VkInstance& instance, VkSurfaceKHR* surface)
	{
		CLF_ASSERT(glfwCreateWindowSurface(instance, window, nullptr, surface) == VK_SUCCESS, "Failed to create Window Surface");
	}
}