#pragma once
#include "Core.hpp"
#include "Pipeline.hpp"
#include "Swapchain.hpp"
#include "Device.hpp"

namespace clf
{
	class Renderer
	{
	public:
		Renderer(Pipeline& pipeline, Swapchain& swapchain, Device& device);
		~Renderer();

		void DrawFrame();

	private:
		Pipeline& pipeline;
		Swapchain& swapchain;
		Device& device;

		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;

		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;
		VkFence inFlightFence;

		void InitCommandPool();
		void InitCommandBuffer();
		void InitSyncObjects();

		void SetCommandBuffer(const u32& imageIndex);
	};
}