#include "pipeline_state.h"
#include "error.h"
#include <array>
#include <vector>
#include <cassert>

namespace Rc
{
    // Pipeline

    Pipeline::Pipeline(VulkanDevice const& vk_device, VkGraphicsPipelineCreateInfo const& create_info) :
        m_vk_device{&vk_device}
    {
        m_vk_pipeline = m_vk_device->CreateGraphicsPipeline(VK_NULL_HANDLE, create_info);
    }

    Pipeline::~Pipeline()
    {
        if (m_vk_device)
        {
            m_vk_device->DestroyPipeline(m_vk_pipeline);
        }
    }

    // PipelineLayout

    PipelineLayout::PipelineLayout(VulkanDevice const& vk_device) :
        m_vk_device{&vk_device}
    {
        VkPipelineLayoutCreateInfo const create_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
        };

        m_vk_pipeline_layout = m_vk_device->CreatePipelineLayout(create_info);
    }

    PipelineLayout::~PipelineLayout()
    {
        if (m_vk_device)
        {
            m_vk_device->DestroyPipelineLayout(m_vk_pipeline_layout);
        }
    }

    // PipelineFactory

    PipelineFactory::PipelineFactory(VulkanDevice const& vk_device) : m_vk_device{&vk_device}
    {
        // VkPipelineInputAssemblyStateCreateInfo
        m_vk_input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        m_vk_input_assembly.pNext = nullptr;
        m_vk_input_assembly.flags = 0;
        m_vk_input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        m_vk_input_assembly.primitiveRestartEnable = VK_FALSE;

        // VkPipelineViewportStateCreateInfo
        m_vk_viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        m_vk_viewport_state.pNext = nullptr;
        m_vk_viewport_state.flags = 0;
        m_vk_viewport_state.viewportCount = 1;
        m_vk_viewport_state.pViewports = nullptr;
        m_vk_viewport_state.scissorCount = 1;
        m_vk_viewport_state.pScissors = nullptr;

        // VkPipelineRasterizationStateCreateInfo
        m_vk_rasterizer_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        m_vk_rasterizer_state.pNext = nullptr;
        m_vk_rasterizer_state.flags = 0;
        m_vk_rasterizer_state.depthClampEnable = VK_FALSE;
        m_vk_rasterizer_state.rasterizerDiscardEnable = VK_FALSE;
        m_vk_rasterizer_state.polygonMode = VK_POLYGON_MODE_FILL;
        m_vk_rasterizer_state.cullMode = VK_CULL_MODE_BACK_BIT;
        m_vk_rasterizer_state.frontFace = VK_FRONT_FACE_CLOCKWISE;
        m_vk_rasterizer_state.depthBiasEnable = VK_FALSE;
        m_vk_rasterizer_state.depthBiasConstantFactor = 0.0f;
        m_vk_rasterizer_state.depthBiasClamp = 0.0f;
        m_vk_rasterizer_state.depthBiasSlopeFactor = 1.0f;
        m_vk_rasterizer_state.lineWidth = 1.0f;

        // VkPipelineMultisampleStateCreateInfo
        m_vk_multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        m_vk_multisampling.pNext = nullptr;
        m_vk_multisampling.flags = 0;
        m_vk_multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        m_vk_multisampling.sampleShadingEnable = VK_FALSE;
        m_vk_multisampling.minSampleShading = 1.0f;
        m_vk_multisampling.pSampleMask = nullptr;
        m_vk_multisampling.alphaToCoverageEnable = VK_FALSE;
        m_vk_multisampling.alphaToOneEnable = VK_FALSE;

        // VkPipelineColorBlendAttachmentState
        m_vk_color_blend_attachment.blendEnable = VK_FALSE;
        m_vk_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        m_vk_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        m_vk_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        m_vk_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        m_vk_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        m_vk_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
        m_vk_color_blend_attachment.colorWriteMask = {
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT
        };

        // VkPipelineColorBlendStateCreateInfo
        m_vk_color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        m_vk_color_blend.pNext = nullptr;
        m_vk_color_blend.flags = 0;
        m_vk_color_blend.logicOpEnable = VK_FALSE;
        m_vk_color_blend.logicOp = VK_LOGIC_OP_COPY;
        m_vk_color_blend.attachmentCount = 1;
        m_vk_color_blend.pAttachments = &m_vk_color_blend_attachment;

        // VkPipelineRenderingCreateInfo
        m_vk_pipeline_rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        m_vk_pipeline_rendering.pNext = nullptr;
        m_vk_pipeline_rendering.viewMask = 0;
        m_vk_pipeline_rendering.colorAttachmentCount = 1; //-------------------------- default should be 0
        m_vk_pipeline_rendering.pColorAttachmentFormats = &m_vk_format;
        m_vk_pipeline_rendering.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
        m_vk_pipeline_rendering.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
    }

    std::unique_ptr<Pipeline> PipelineFactory::Create()
    {
        assert(m_vk_pipeline_layout != nullptr);

        std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
        
        if (m_vk_vs != nullptr)
        {
            shader_stages.emplace_back(
                VkPipelineShaderStageCreateInfo
                {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = m_vk_vs,
                    .pName = "main",
                    .pSpecializationInfo = nullptr
                }
            );
        }
        if (m_vk_ps != nullptr)
        {
            shader_stages.emplace_back(
                VkPipelineShaderStageCreateInfo
                {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = m_vk_ps,
                    .pName = "main",
                    .pSpecializationInfo = nullptr
                }
            );
        }

        // See Dynamic State tutorial ??????????????????????????????????????????
        std::array<VkDynamicState, 2> const dynamic_states
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo const dynamic_state_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
            .pDynamicStates = dynamic_states.data()
        };

        VkPipelineVertexInputStateCreateInfo vertex_input_info {};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexAttributeDescriptionCount = m_vk_vertex_input_arrtibutes.size();
        vertex_input_info.pVertexAttributeDescriptions = m_vk_vertex_input_arrtibutes.data();

        if (m_vk_vertex_binding_desc.stride > 0)
        {
            vertex_input_info.vertexBindingDescriptionCount = 1;
            vertex_input_info.pVertexBindingDescriptions = &m_vk_vertex_binding_desc;
        }
        else
        {
            vertex_input_info.vertexBindingDescriptionCount = 0;
            vertex_input_info.pVertexBindingDescriptions = nullptr;
        }

        VkGraphicsPipelineCreateInfo const pipeline_info
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &m_vk_pipeline_rendering,
            .flags = 0,
            .stageCount = static_cast<uint32_t>(shader_stages.size()),
            .pStages = shader_stages.data(),
            .pVertexInputState = &vertex_input_info,
            .pInputAssemblyState = &m_vk_input_assembly,
            .pTessellationState = nullptr,
            .pViewportState = &m_vk_viewport_state,
            .pRasterizationState = &m_vk_rasterizer_state,
            .pMultisampleState = &m_vk_multisampling,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &m_vk_color_blend,
            .pDynamicState = &dynamic_state_info,
            .layout = m_vk_pipeline_layout->Handle(),
            .renderPass = nullptr,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0
        };

        return std::make_unique<Pipeline>(*m_vk_device, pipeline_info);
    }

    void PipelineFactory::SetVertexInputBinding(std::size_t stride)
    {
        m_vk_vertex_binding_desc.binding = 0;
        m_vk_vertex_binding_desc.stride = stride;
        m_vk_vertex_binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }

} // Rc