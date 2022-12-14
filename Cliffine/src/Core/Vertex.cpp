#include "Vertex.hpp"

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

	VertexBufferLayout::VertexBufferLayout(const span<const AttributeType>& types) 
	{
		for (u32 i = 0; i < types.size(); i++)
			attributes[i] = { i++, 0, ToFormat(types[i]), stride += GetSize(types[i]) };

		bindingDescription = {0, stride, VK_VERTEX_INPUT_RATE_VERTEX};
	}
}