#pragma once
#include "Core.hpp"
#include "Pipeline.hpp"
#include "Swapchain.hpp"
#include "Device.hpp"
#include "Model.hpp"

namespace clf
{
	class Renderer
	{
	public:
		Renderer(Pipeline& pipeline, Swapchain& swapchain, Device& device, Model& model);
		~Renderer();

		void DrawFrame();

	private:
		Pipeline& pipeline;
		Swapchain& swapchain;
		Device& device;
		Model& model;

		VkCommandPool commandPool;
		vec<VkCommandBuffer> commandBuffers;

		vec<VkSemaphore> imageAvailableSemaphores;
		vec<VkSemaphore> renderFinishedSemaphores;
		vec<VkFence> inFlightFences;

		u32 currentFrame;

		void InitCommandPool();
		void InitCommandBuffers();
		void InitSyncObjects();

		void SetCommandBuffer();
	};
}