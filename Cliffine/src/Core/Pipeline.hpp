#pragma once
#include "Core.hpp"
#include "Device.hpp"
#include "Swapchain.hpp"

namespace clf
{
	class Pipeline
	{
	public:
		Pipeline(Device& device, Swapchain& swapchain);
		~Pipeline();

		VkPipeline& GetNative() { return pipeline; }

	private:
		static const vec<char> ReadFile(const std::string& path);

		Device& device;
		Swapchain& swapchain;

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;

		void InitPipeline();

		VkShaderModule CreateShaderModule(const vec<char>& code);
	};
}