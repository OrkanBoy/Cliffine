#include "Renderer.hpp"

namespace clf
{
	Renderer::Renderer(Pipeline& pipeline, Swapchain& swapchain, Device& device, Model& model)
		: pipeline(pipeline), swapchain(swapchain), device(device), model(model), currentFrame(0)
	{
		InitCommandPool();
		InitCommandBuffers();
		InitSyncObjects();
	}

	Renderer::~Renderer()
	{ 
		vkDestroyCommandPool(device.GetLogicalDevice(), commandPool, nullptr);
		for (auto& ias : imageAvailableSemaphores)
			vkDestroySemaphore(device.GetLogicalDevice(), ias, nullptr);
		for (auto& rfs : renderFinishedSemaphores)
			vkDestroySemaphore(device.GetLogicalDevice(), rfs, nullptr);
		for (auto& iff : inFlightFences)
			vkDestroyFence(device.GetLogicalDevice(), iff, nullptr);
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

	void Renderer::InitCommandBuffers()
	{
		commandBuffers.resize(swapchain.GetImageCount());
		VkCommandBufferAllocateInfo allocInfo{};
		
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (u32)commandBuffers.size();
		
		CLF_ASSERT(vkAllocateCommandBuffers(device.GetLogicalDevice(), &allocInfo, commandBuffers.data()) == VK_SUCCESS,
				"failed to allocate command buffers!");
	}

	void Renderer::InitSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		u32 imageCount = swapchain.GetImageCount();
		imageAvailableSemaphores.resize(imageCount);
		renderFinishedSemaphores.resize(imageCount);
		inFlightFences.resize(imageCount);

		for (u32 i = 0; i < imageCount; i++)
			CLF_ASSERT(vkCreateSemaphore(device.GetLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) == VK_SUCCESS &&
				vkCreateSemaphore(device.GetLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) == VK_SUCCESS &&
				vkCreateFence(device.GetLogicalDevice(), &fenceInfo, nullptr, &inFlightFences[i]) == VK_SUCCESS,
				"Failed to initialize Sync Objects!");
	}

	void Renderer::DrawFrame()
	{
		vkWaitForFences(device.GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		VkResult result = vkAcquireNextImageKHR(device.GetLogicalDevice(), swapchain.GetNative(), UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr, &currentFrame);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			swapchain.Reinit();
			currentFrame = 0;
			return;
		}
		else CLF_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR,
			"Failed to acquire swapchain image!");

		vkResetFences(device.GetLogicalDevice(), 1, &inFlightFences[currentFrame]);

		vkResetCommandBuffer(commandBuffers[currentFrame], 0);
		SetCommandBuffer();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];

		CLF_ASSERT(vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) == VK_SUCCESS,
			"Failed to submit draw Command Buffer!");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain.GetNative();
		presentInfo.pImageIndices = &currentFrame;

		result = vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || device.GetWindow().framebufferResized)
		{
			swapchain.Reinit();
			currentFrame = 0;
			return;
		}
		else CLF_ASSERT(result == VK_SUCCESS, 
			"Failed to acquire present image!");
		
		currentFrame = (currentFrame + 1) % swapchain.GetImageCount();
	}

	void Renderer::SetCommandBuffer()
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		VkCommandBuffer& commandBuffer = commandBuffers[currentFrame];

		CLF_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS,
			"Failed to begin recording Command Buffer!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapchain.GetRenderPass();
		renderPassInfo.framebuffer = swapchain.GetFramebuffer(currentFrame);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain.GetExtent();

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
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

		model.Bind(commandBuffer);
		model.Draw(commandBuffer);

		vkCmdEndRenderPass(commandBuffer);
		CLF_ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS,
			"Failed to record Command Buffer!");
	}
}