#include "Model.hpp"

namespace clf
{
	Model::Model(Device& device, const VertexLayout& vertexLayout, void* data, const u32& count)
		: device(device), vertexLayout(vertexLayout), count(count)
	{
		InitVertexBuffer(data);
	}

	Model::~Model() 
	{
		vkDestroyBuffer(device.GetLogicalDevice(), vertexBuffer, nullptr);
		vkFreeMemory(device.GetLogicalDevice(), vertexMemory, nullptr);
	}

	void Model::InitVertexBuffer(void* data)
	{
		VkDeviceSize bufferSize = vertexLayout.stride * count;
		device.CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexMemory);

		vkMapMemory(device.GetLogicalDevice(), vertexMemory, 0, bufferSize, 0, &this->data);
		memcpy(this->data, data, bufferSize);
		vkUnmapMemory(device.GetLogicalDevice(), vertexMemory);
	}

	void Model::Draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, count, count / 3, 0, 0);
	}

	void Model::Bind(VkCommandBuffer commandBuffer)
	{
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);
	}
}