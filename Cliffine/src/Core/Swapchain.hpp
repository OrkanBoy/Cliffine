#pragma once
#include "Core.hpp"
#include "Window.hpp"
#include "Device.hpp"

namespace clf
{
	class Swapchain
	{
	public:
		Swapchain(Device& device);
		~Swapchain();

		void Reinit();

		VkFramebuffer& GetFramebuffer(const u32& imageIndex) { return framebuffers[imageIndex]; }
		VkSwapchainKHR& GetNative() { return swapchain; }
		const u32 GetWidth() const { return extent.width; }
		const u32 GetHeight() const { return extent.height; }
		const VkExtent2D GetExtent() const { return extent; }
		const VkFormat GetImageFormat() const { return imageFormat; }
		const VkRenderPass GetRenderPass() const { return renderPass; }
		const u32 GetImageCount() const { return imageCount; }

	private:

		VkSwapchainKHR swapchain;

		Device& device;

		VkFormat imageFormat;
		vec<VkFramebuffer> framebuffers;
		vec<VkImageView> imageViews;
		vec<VkImage> images;

		u32 imageCount;
		VkRenderPass renderPass;
		VkExtent2D extent;

		void InitSwapchain();
		void InitImageViews();
		void InitRenderPass();
		void InitFramebuffers();

		VkSurfaceFormatKHR ChooseSurfaceFormat(const vec<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChoosePresentMode(const vec<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	};
}