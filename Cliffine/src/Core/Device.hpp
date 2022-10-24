#pragma once
#include "Core.hpp"
#include "Window.hpp"

namespace clf
{
	struct QueueFamilies
	{
		std::optional<u32> graphics;
		std::optional<u32> present;

		bool IsComplete() const
		{
			return graphics.has_value() && present.has_value();
		}
	};

	struct SwapchainDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;

		bool IsAdequate() const
		{
			return !formats.empty() && !presentModes.empty();
		}
	};

	class Device
	{
	public:
		Device(Window& window);
		~Device();

		const arr<const char*, 1> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		const arr<const char*, 1> validationLayers = {
			"VK_LAYER_KHRONOS_validation" };

		void WaitIdle() { vkDeviceWaitIdle(logicalDevice); }

		const SwapchainDetails GetSwapchainDetails() const { return FindSwapchainDetails(physicalDevice); }
		const QueueFamilies GetQueueFamilies() const { return FindQueueFamilies(physicalDevice); }
		Window& GetWindow() { return window; }
		VkSurfaceKHR& GetSurface() { return surface; }
		VkDevice& GetLogicalDevice() { return logicalDevice; }
		VkQueue& GetGraphicsQueue() { return graphicsQueue; }
		VkQueue& GetPresentQueue() { return presentQueue; }

		void CreateBuffer(
			const VkDeviceSize& size,
			const VkBufferUsageFlags& usage,
			const VkMemoryPropertyFlags& props,
			VkBuffer& outBuffer,
			VkDeviceMemory& outMemory);
	private:
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

		Window& window;
		VkSurfaceKHR surface;

		VkInstance instance;

		VkDebugUtilsMessengerEXT debugMessenger;

		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
		VkQueue graphicsQueue;
		VkQueue presentQueue;

		void InitVulkan();
		void InitDebugMessenger();
		void InitSurface();
		void InitPhysicalDevice();
		void InitLogicalDevice();

		VkDebugUtilsMessengerCreateInfoEXT CreateDebugMessengerInfo();

		void CheckValidationLayerSupport() const;
		void CheckExtensionSupport() const;

		const bool IsDeviceSuited(const VkPhysicalDevice& device) const;
		const bool IsDeviceExtensionSupported(const VkPhysicalDevice& device) const;

		const QueueFamilies FindQueueFamilies(const VkPhysicalDevice& device) const;
		const SwapchainDetails FindSwapchainDetails(const VkPhysicalDevice& device) const;
		const u32 FindMemoryType(u32 typeFilter, VkMemoryPropertyFlags props) const;

		const vec<const char*> GetRequiredExtensions() const;
	};
}