#pragma once
#include "VertexLayout.hpp"
#include "Device.hpp"

namespace clf
{
	class Model
	{
	public:
		Model(Device& device, const VertexLayout& vertexLayout, void* data, const u32& count);
		~Model();

		void Draw(VkCommandBuffer commandBuffer);
		void Bind(VkCommandBuffer commandBuffer);

	private:
		Device& device;

		void* data;
		u32 count;

		VertexLayout vertexLayout;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexMemory;

		void InitVertexBuffer(void* data);
	};
}