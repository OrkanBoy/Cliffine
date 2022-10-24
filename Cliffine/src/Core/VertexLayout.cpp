#include "VertexLayout.hpp"

namespace clf
{
	VkFormat ToFormat(const AttributeType type)
	{
		using enum AttributeType;

		switch (type)
		{
			case Float: return VK_FORMAT_R32_SFLOAT;
			case Float2: return VK_FORMAT_R32G32_SFLOAT;
			case Float3: return VK_FORMAT_R32G32B32_SFLOAT;
			case Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;

			case Int: return VK_FORMAT_R32_SINT;
			case Int2: return VK_FORMAT_R32G32_SINT;
			case Int3: return VK_FORMAT_R32G32B32_SINT;
			case Int4: return VK_FORMAT_R32G32B32A32_SINT;
		}
	}

	u32 GetSize(const AttributeType type)
	{
		using enum AttributeType;

		switch (type)
		{
			case Float:
			case Int:
				return 4;
			case Float2:
			case Int2:
				return 4 * 2;
			case Float3:
			case Int3:
				return 4 * 3;
			case Float4:
			case Int4: 
				return 4 * 4;
		}
	}

	VertexLayout::VertexLayout(const std::initializer_list<const AttributeType> types)
		: stride(0)
	{
		u32 i = 0;
		attributes.resize(types.size());

		for (auto& type : types)
		{
			VkVertexInputAttributeDescription attribute;
			attribute.binding = 0;
			attribute.format = ToFormat(type);
			attribute.location = i;
			attribute.offset = stride;

			attributes[i] = attribute;
			stride += GetSize(type);
			i++;
		}

		bindingDescription = {0, stride, VK_VERTEX_INPUT_RATE_VERTEX};
	}
}