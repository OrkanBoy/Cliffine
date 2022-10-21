#include "Swapchain.hpp"

namespace clf
{
	Swapchain::Swapchain(Device& device)
		: device(device)
	{
		InitSwapchain();
		InitImageViews();
		InitRenderPass();
		InitFramebuffers();
	}

	Swapchain::~Swapchain()
	{
		for (auto framebuffer : framebuffers) {
			vkDestroyFramebuffer(device.GetLogicalDevice(), framebuffer, nullptr);
		}
		vkDestroySwapchainKHR(device.GetLogicalDevice(), swapchain, nullptr);
		for (auto& imageView : imageViews)
			vkDestroyImageView(device.GetLogicalDevice(), imageView, nullptr);
		vkDestroyRenderPass(device.GetLogicalDevice(), renderPass, nullptr);
	}

	void Swapchain::InitSwapchain()
	{
		SwapchainDetails details = device.GetSwapchainDetails();

		VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(details.formats);
		VkPresentModeKHR presentMode = ChoosePresentMode(details.presentModes);
		VkExtent2D extent = ChooseExtent(details.capabilities);


		imageCount = details.capabilities.minImageCount + 1;
		if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
			imageCount = details.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = device.GetSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //Post-processing: VK_IMAGE_USAGE_TRANSFER_DST_BIT 


		QueueFamilies families = device.GetQueueFamilies();
		uint32_t queueFamilyIndices[]{ families.graphics.value(), families.present.value() };

		if (families.graphics != families.present)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

		createInfo.preTransform = details.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = nullptr;

		CLF_ASSERT(vkCreateSwapchainKHR(device.GetLogicalDevice(), &createInfo, nullptr, &swapchain) == VK_SUCCESS,
			"Failed to initialize swap chain!");

		images.resize(imageCount);
		vkGetSwapchainImagesKHR(device.GetLogicalDevice(), swapchain, &imageCount, images.data());

		imageFormat = surfaceFormat.format;
		this->extent = extent;
	}

	void Swapchain::InitImageViews()
	{
		imageViews.resize(imageCount);

		for (u32 i = 0; i < imageCount; i++)
		{
			VkImageViewCreateInfo createInfo {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = images[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = imageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			CLF_ASSERT(vkCreateImageView(device.GetLogicalDevice(), &createInfo, nullptr, &imageViews[i]) == VK_SUCCESS,
				"Failed to create Image View!");
		}


	}

	void Swapchain::InitRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = imageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &colorAttachment;
		createInfo.subpassCount = 1;
		createInfo.pSubpasses = &subpass;
		createInfo.dependencyCount = 1;
		createInfo.pDependencies = &dependency;

		CLF_ASSERT(vkCreateRenderPass(device.GetLogicalDevice(), &createInfo, nullptr, &renderPass) == VK_SUCCESS,
			"Failed to initialize Render Pass!");
	}

	void Swapchain::InitFramebuffers()
	{
		framebuffers.resize(imageCount);

		for (u32 i = 0; i < imageCount; i++)
		{
			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = &imageViews[i];
			framebufferInfo.width = extent.width;
			framebufferInfo.height = extent.height;
			framebufferInfo.layers = 1;

			CLF_ASSERT(vkCreateFramebuffer(device.GetLogicalDevice(), &framebufferInfo, nullptr, &framebuffers[i]) == VK_SUCCESS ,
				"Failed to initialize framebuffer!");
		}
	}

	VkSurfaceFormatKHR Swapchain::ChooseSurfaceFormat(const vec<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;

		return availableFormats[0];
	}

	VkPresentModeKHR Swapchain::ChoosePresentMode(const vec<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D Swapchain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<u32>::max())
			return capabilities.currentExtent;
		else
		{
			i32 width, height;
			glfwGetFramebufferSize(&device.GetWindow().GetNative(), &width, &height);

			return
			{
				std::clamp((u32)width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
				std::clamp((u32)height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
			};
		}

	}
}