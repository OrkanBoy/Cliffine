#include "Renderer.hpp"

namespace clf
{
	Renderer::Renderer(Pipeline& pipeline, Swapchain& swapchain, Device& device)
		: pipeline(pipeline), swapchain(swapchain), device(device)
	{
		InitCommandPool();
		InitCommandBuffer();
		InitSyncObjects();
	}

	Renderer::~Renderer()
	{ 
		vkDestroyCommandPool(device.GetLogicalDevice(), commandPool, nullptr);
		vkDestroySemaphore(device.GetLogicalDevice(), imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(device.GetLogicalDevice(), renderFinishedSemaphore, nullptr);
		vkDestroyFence(device.GetLogicalDevice(), inFlightFence, nullptr);
	}

	void Renderer::InitCommandPool()
	{
		QueueFamilies families = device.GetQueueFamilies();

		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		createInfo.queueFamilyIndex = families.graphics.value();

		CLF_ASSERT(vkCreateCommandPool(device.GetLogicalDevice(), &createInfo, nullptr, &commandPool) == VK_SUCCESS, 
			"Failed to initialize Command Pool!");
	}

	void Renderer::InitCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		CLF_ASSERT(vkAllocateCommandBuffers(device.GetLogicalDevice(), &allocInfo, &commandBuffer) == VK_SUCCESS,
			"failed to allocate command buffers!");
	}

	void Renderer::InitSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		CLF_ASSERT(vkCreateSemaphore(device.GetLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) == VK_SUCCESS &&
			vkCreateSemaphore(device.GetLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) == VK_SUCCESS &&
			vkCreateFence(device.GetLogicalDevice(), &fenceInfo, nullptr, &inFlightFence) == VK_SUCCESS,
			"Failed to initialize Sync Objects!");
	}

	void Renderer::DrawFrame()
	{
		vkWaitForFences(device.GetLogicalDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(device.GetLogicalDevice(), 1, &inFlightFence);

		u32 imageIndex;
		vkAcquireNextImageKHR(device.GetLogicalDevice(), swapchain.GetNative(), UINT64_MAX, imageAvailableSemaphore, nullptr, &imageIndex);

		vkResetCommandBuffer(commandBuffer, 0);
		SetCommandBuffer(imageIndex);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphore};
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		CLF_ASSERT(vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, inFlightFence) == VK_SUCCESS,
			"Failed to submit draw Command Buffer!");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain.GetNative();
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);
	}

	void Renderer::SetCommandBuffer(const u32& imageIndex) 
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		CLF_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS,
			"Failed to begin recording Command Buffer!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapchain.GetRenderPass();
		renderPassInfo.framebuffer = swapchain.GetFramebuffer(imageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain.GetExtent();

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetNative());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapchain.GetWidth();
		viewport.height = (float)swapchain.GetHeight();
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchain.GetExtent();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDraw(commandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);
		CLF_ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS,
			"Failed to record Command Buffer!");
	}
}