#include "pipeline_state.h"
#include <array>
#include <vector>
#include <cassert>

namespace Rc::Render
{
    // Pipeline

    Pipeline::Pipeline(VulkanDevice const& vk_device, VkGraphicsPipelineCreateInfo const& create_info) :
        m_vk_device{&vk_device}
    {
        m_vk_pipeline = m_vk_device->CreateGraphicsPipeline(VK_NULL_HANDLE, create_info);
    }

    Pipeline::~Pipeline()
    {
        if (m_vk_device != nullptr)
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
        if (m_vk_device != nullptr)
        {
            m_vk_device->DestroyPipelineLayout(m_vk_pipeline_layout);
        }
    }

    // PipelineFactory

    PipelineFactory::PipelineFactory(VulkanDevice const& vk_device) : m_device{&vk_device}
    {
        // VkPipelineInputAssemblyStateCreateInfo
        m_input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        m_input_assembly.pNext = nullptr;
        m_input_assembly.flags = 0;
        m_input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        m_input_assembly.primitiveRestartEnable = VK_FALSE;

        // VkPipelineViewportStateCreateInfo
        m_viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        m_viewport_state.pNext = nullptr;
        m_viewport_state.flags = 0;
        m_viewport_state.viewportCount = 1;
        m_viewport_state.pViewports = nullptr;
        m_viewport_state.scissorCount = 1;
        m_viewport_state.pScissors = nullptr;

        // VkPipelineRasterizationStateCreateInfo
        m_rasterizer_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        m_rasterizer_state.pNext = nullptr;
        m_rasterizer_state.flags = 0;
        m_rasterizer_state.depthClampEnable = VK_FALSE;
        m_rasterizer_state.rasterizerDiscardEnable = VK_FALSE;
        m_rasterizer_state.polygonMode = VK_POLYGON_MODE_FILL;
        m_rasterizer_state.cullMode = VK_CULL_MODE_BACK_BIT;
        m_rasterizer_state.frontFace = VK_FRONT_FACE_CLOCKWISE;
        m_rasterizer_state.depthBiasEnable = VK_FALSE;
        m_rasterizer_state.depthBiasConstantFactor = 0.0f;
        m_rasterizer_state.depthBiasClamp = 0.0f;
        m_rasterizer_state.depthBiasSlopeFactor = 1.0f;
        m_rasterizer_state.lineWidth = 1.0f;

        // VkPipelineMultisampleStateCreateInfo
        m_multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        m_multisampling.pNext = nullptr;
        m_multisampling.flags = 0;
        m_multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        m_multisampling.sampleShadingEnable = VK_FALSE;
        m_multisampling.minSampleShading = 1.0f;
        m_multisampling.pSampleMask = nullptr;
        m_multisampling.alphaToCoverageEnable = VK_FALSE;
        m_multisampling.alphaToOneEnable = VK_FALSE;

        // VkPipelineColorBlendAttachmentState
        m_color_blend_attachment.blendEnable = VK_FALSE;
        m_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        m_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        m_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        m_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        m_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        m_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
        m_color_blend_attachment.colorWriteMask = {
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT
        };

        // VkPipelineColorBlendStateCreateInfo
        m_color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        m_color_blend.pNext = nullptr;
        m_color_blend.flags = 0;
        m_color_blend.logicOpEnable = VK_FALSE;
        m_color_blend.logicOp = VK_LOGIC_OP_COPY;
        m_color_blend.attachmentCount = 1;
        m_color_blend.pAttachments = &m_color_blend_attachment;

        // VkPipelineRenderingCreateInfo
        m_pipeline_rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        m_pipeline_rendering.pNext = nullptr;
        m_pipeline_rendering.viewMask = 0;
        m_pipeline_rendering.colorAttachmentCount = 1; //-------------------------- default should be 0
        m_pipeline_rendering.pColorAttachmentFormats = &m_format;
        m_pipeline_rendering.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
        m_pipeline_rendering.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

        for (auto& desc : m_vertex_binding_desc)
        {
            desc.stride = 0;
            desc.binding = 0;
            desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        }
    }

    std::unique_ptr<Pipeline> PipelineFactory::Create()
    {
        assert(m_pipeline_layout != nullptr);

        std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
        
        if (m_vs != nullptr)
        {
            shader_stages.emplace_back(
                VkPipelineShaderStageCreateInfo
                {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = m_vs,
                    .pName = "main",
                    .pSpecializationInfo = nullptr
                }
            );
        }
        if (m_ps != nullptr)
        {
            shader_stages.emplace_back(
                VkPipelineShaderStageCreateInfo
                {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = m_ps,
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

        auto vertex_input_arrtibutes = m_vertex_arrtibutes;
        vertex_input_arrtibutes.insert_range(vertex_input_arrtibutes.end(), m_instance_arrtibutes);

        VkPipelineVertexInputStateCreateInfo vertex_input_info {};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexAttributeDescriptionCount = vertex_input_arrtibutes.size();
        vertex_input_info.pVertexAttributeDescriptions = vertex_input_arrtibutes.data();

        // Setup vertex input binding...

        std::vector<VkVertexInputBindingDescription> vk_vertex_binding_desc;

        for (auto& desc : m_vertex_binding_desc)
        {
            if (desc.stride > 0)
            {
                vk_vertex_binding_desc.push_back(desc);
            }
        }

        if (vk_vertex_binding_desc.empty())
        {
            vertex_input_info.vertexBindingDescriptionCount = 0;
            vertex_input_info.pVertexBindingDescriptions = nullptr;
        }
        else
        {
            vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(vk_vertex_binding_desc.size());
            vertex_input_info.pVertexBindingDescriptions = vk_vertex_binding_desc.data();
        }

        VkGraphicsPipelineCreateInfo const pipeline_info
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &m_pipeline_rendering,
            .flags = 0,
            .stageCount = static_cast<uint32_t>(shader_stages.size()),
            .pStages = shader_stages.data(),
            .pVertexInputState = &vertex_input_info,
            .pInputAssemblyState = &m_input_assembly,
            .pTessellationState = nullptr,
            .pViewportState = &m_viewport_state,
            .pRasterizationState = &m_rasterizer_state,
            .pMultisampleState = &m_multisampling,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &m_color_blend,
            .pDynamicState = &dynamic_state_info,
            .layout = m_pipeline_layout->Handle(),
            .renderPass = nullptr,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0
        };

        return std::make_unique<Pipeline>(*m_device, pipeline_info);
    }

    static VkFormat GetFormat(Gfx::VertexAttribute attribute)
    {
        switch (attribute)
        {
            case Gfx::VertexAttribute::Position:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case Gfx::VertexAttribute::Normal:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case Gfx::VertexAttribute::Color:
                return VK_FORMAT_R32G32B32_SFLOAT;
        }

        std::unreachable();
    }

    static uint32_t GetLocation(Gfx::VertexAttribute attribute)
    {
        switch (attribute)
        {
            case Gfx::VertexAttribute::Position:
                return 0;
            case Gfx::VertexAttribute::Normal:
                return 1;
            case Gfx::VertexAttribute::Color:
                return 2;
        }

        std::unreachable();
    }

    void PipelineFactory::SetVertexInput(std::size_t stride, std::span<Gfx::VertexDescription const> attributes)
    {
        constexpr uint32_t binding = 0;

        m_vertex_binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        m_vertex_binding_desc[0].binding = binding;
        m_vertex_binding_desc[0].stride = stride;

        m_vertex_arrtibutes.clear();
        m_vertex_arrtibutes.reserve(attributes.size());

        for (auto const& a : attributes)
        {
            m_vertex_arrtibutes.push_back({
                .location = GetLocation(a.attribute),
                .binding = binding,
                .format = GetFormat(a.attribute),
                .offset = a.offset
            });
        }
    }

    void PipelineFactory::SetInstanceInput(std::size_t stride, std::span<Gfx::VertexDescription const> attributes)
    {
        constexpr uint32_t binding = 1;

        m_vertex_binding_desc[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
        m_vertex_binding_desc[1].binding = binding;
        m_vertex_binding_desc[1].stride = stride;

        for (auto const& a : attributes)
        {
            m_vertex_arrtibutes.push_back({
                .location = GetLocation(a.attribute),
                .binding = binding,
                .format = GetFormat(a.attribute),
                .offset = a.offset
            });
        }
    }

} // Rc::Render
