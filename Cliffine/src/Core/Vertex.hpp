#pragma once
#define CLF_CODE(t) typeid(t).hash_code()

#include "Core.hpp"

#include <glm/glm.hpp>
#include <typeinfo>

namespace clf
{
    enum class AttributeType
    {
        Float, Float2, Float3, Float4,
        Int, Int2, Int3, Int4,
        Mat4x4, Mat3x3, Mat2x2, 
        Bool
    };

    VkFormat ToFormat(const AttributeType type);
    u32 GetSize(const AttributeType type);

    struct VertexBufferLayout
    {
        VertexBufferLayout(const span<const AttributeType>& types);

        u32 stride = 0;
        vec<VkVertexInputAttributeDescription> attributes;
        VkVertexInputBindingDescription bindingDescription;
    };
}