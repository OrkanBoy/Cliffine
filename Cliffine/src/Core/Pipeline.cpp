#include "Pipeline.hpp"

#include <fstream>

namespace clf
{
	Pipeline::Pipeline(Device& device, Swapchain& swapchain, const VertexLayout& vertexLayout)
		: device(device), swapchain(swapchain), vertexLayout(vertexLayout)
	{
		InitPipeline();
	}

	Pipeline::~Pipeline()
	{
        vkDestroyPipeline(device.GetLogicalDevice(), pipeline, nullptr);
        vkDestroyPipelineLayout(device.GetLogicalDevice(), pipelineLayout, nullptr);
    }

	const vec<char> Pipeline::ReadFile(const std::string& path)
	{
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        CLF_ASSERT(file.is_open(), "Failed to open file: {0}", path)

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(file.tellg());

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
	}

	void Pipeline::InitPipeline()
	{
		auto vertCode = ReadFile("shaders/.vert.spv"),
			fragCode = ReadFile("shaders/.frag.spv");

		VkShaderModule vertModule = CreateShaderModule(vertCode),
			fragModule = CreateShaderModule(fragCode);
		
		VkPipelineShaderStageCreateInfo vertStageInfo{},
			fragStageInfo{};


		const arr<VkShaderStageFlagBits, 2> shaderStages{ VK_SHADER_STAGE_VERTEX_BIT , VK_SHADER_STAGE_FRAGMENT_BIT };
		arr<VkShaderModule, 2> modules{ vertModule, fragModule };
		arr<VkPipelineShaderStageCreateInfo, 2> stageInfos{ vertStageInfo, fragStageInfo };
		
		for (u8 i = 0; i < stageInfos.size(); i++)
		{
			stageInfos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stageInfos[i].stage = shaderStages[i];
			stageInfos[i].module = modules[i];
			stageInfos[i].pName = "main";
		}
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = (u32)vertexLayout.attributes.size();
        vertexInputInfo.pVertexBindingDescriptions = &vertexLayout.bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = vertexLayout.attributes.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        CLF_ASSERT(vkCreatePipelineLayout(device.GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) == VK_SUCCESS,
            "Failed to initialize pipeline layout!");

        
        VkGraphicsPipelineCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.stageCount = (u32)stageInfos.size();
        createInfo.pStages = stageInfos.data();
        createInfo.pVertexInputState = &vertexInputInfo;
        createInfo.pInputAssemblyState = &inputAssembly;
        createInfo.pViewportState = &viewportState;
        createInfo.pRasterizationState = &rasterizer;
        createInfo.pMultisampleState = &multisampling;
        createInfo.pColorBlendState = &colorBlending;
        createInfo.pDynamicState = &dynamicState;
        createInfo.layout = pipelineLayout;
        createInfo.renderPass = swapchain.GetRenderPass();
        createInfo.subpass = 0;

        CLF_ASSERT(vkCreateGraphicsPipelines(device.GetLogicalDevice(), nullptr, 1, &createInfo, nullptr, &pipeline) == VK_SUCCESS,
            "Failed to initialize pipeline");
            
        vkDestroyShaderModule(device.GetLogicalDevice(), fragModule, nullptr);
        vkDestroyShaderModule(device.GetLogicalDevice(), vertModule, nullptr);
	}

	VkShaderModule Pipeline::CreateShaderModule(const vec<char>& code)
	{
		VkShaderModuleCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule module;
		CLF_ASSERT(vkCreateShaderModule(device.GetLogicalDevice(), &createInfo, nullptr, &module) == VK_SUCCESS,
			"Failed to create Shader Module");

		return module;
	}
}