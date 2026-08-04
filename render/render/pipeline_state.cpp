#include "pipeline_state.h"
#include <array>
#include <vector>
#include <cassert>

namespace Rc::Render
{
    // Pipeline

    Pipeline::Pipeline(VulkanDevice const& vk_device, VkGraphicsPipelineCreateInfo const& create_info) :
        vk_device{vk_device}
    {
        vk_pipeline = vk_device.CreateGraphicsPipeline(VK_NULL_HANDLE, create_info);
    }

    Pipeline::~Pipeline()
    {
        vk_device.DestroyPipeline(vk_pipeline);
    }

    // PipelineLayout --------------------------------------------------------------------------- Odstranit

    PipelineLayout::PipelineLayout(VulkanDevice const& vk_device) : vk_device{vk_device}
    {
        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .bindingCount = 0,
            .pBindings = nullptr
        };

        vk_descriptor_set_layout = vk_device.CreateDescriptorSetLayout(descriptor_set_layout_info);

        VkPipelineLayoutCreateInfo const pipeline_layout_create_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &vk_descriptor_set_layout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
        };

        vk_pipeline_layout = vk_device.CreatePipelineLayout(pipeline_layout_create_info);
    }

    PipelineLayout::~PipelineLayout()
    {
        vk_device.DestroyDescriptorSetLayout(vk_descriptor_set_layout);
        vk_device.DestroyPipelineLayout(vk_pipeline_layout);
    }

    // PipelineFactory

    PipelineFactory::PipelineFactory(VulkanDevice const& vk_device) : vk_device{vk_device}
    {
        // VkPipelineInputAssemblyStateCreateInfo
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.pNext = nullptr;
        input_assembly.flags = 0;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        // VkPipelineViewportStateCreateInfo
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.pNext = nullptr;
        viewport_state.flags = 0;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = nullptr;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = nullptr;

        // VkPipelineRasterizationStateCreateInfo
        rasterizer_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer_state.pNext = nullptr;
        rasterizer_state.flags = 0;
        rasterizer_state.depthClampEnable = VK_FALSE;
        rasterizer_state.rasterizerDiscardEnable = VK_FALSE;
        rasterizer_state.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer_state.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer_state.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer_state.depthBiasEnable = VK_FALSE;
        rasterizer_state.depthBiasConstantFactor = 0.0f;
        rasterizer_state.depthBiasClamp = 0.0f;
        rasterizer_state.depthBiasSlopeFactor = 1.0f;
        rasterizer_state.lineWidth = 1.0f;

        // VkPipelineMultisampleStateCreateInfo
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.pNext = nullptr;
        multisampling.flags = 0;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        // VkPipelineColorBlendAttachmentState
        for (auto& color_blend_attachment : color_blend_attachments)
        {
            color_blend_attachment.blendEnable = VK_FALSE;
            color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
            color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
            color_blend_attachment.colorWriteMask = {
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT
            };
        }

        // VkPipelineColorBlendStateCreateInfo
        color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend.pNext = nullptr;
        color_blend.flags = 0;
        color_blend.logicOpEnable = VK_FALSE;
        color_blend.logicOp = VK_LOGIC_OP_COPY;
        color_blend.attachmentCount = color_blend_attachments.size();
        color_blend.pAttachments = color_blend_attachments.data();

        // VkPipelineRenderingCreateInfo
        pipeline_rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        pipeline_rendering.pNext = nullptr;
        pipeline_rendering.viewMask = 0;
        pipeline_rendering.colorAttachmentCount = color_attachment_formats.size();
        pipeline_rendering.pColorAttachmentFormats = color_attachment_formats.data();
        pipeline_rendering.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
        pipeline_rendering.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

        for (auto& desc : vertex_binding_desc)
        {
            desc.stride = 0;
            desc.binding = 0;
            desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        }
    }

    std::unique_ptr<Pipeline> PipelineFactory::Create()
    {
        assert(pipeline_layout != nullptr);

        std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
        
        if (vs != nullptr)
        {
            shader_stages.emplace_back(
                VkPipelineShaderStageCreateInfo
                {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = vs,
                    .pName = "main",
                    .pSpecializationInfo = nullptr
                }
            );
        }
        if (ps != nullptr)
        {
            shader_stages.emplace_back(
                VkPipelineShaderStageCreateInfo
                {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = ps,
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

        // auto vertex_input_arrtibutes = m_vertex_arrtibutes;
        // vertex_input_arrtibutes.insert_range(vertex_input_arrtibutes.end(), m_instance_arrtibutes);

        VkPipelineVertexInputStateCreateInfo vertex_input_info {};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexAttributeDescriptionCount = vertex_arrtibutes.size();
        vertex_input_info.pVertexAttributeDescriptions = vertex_arrtibutes.data();

        // Setup vertex input binding...

        std::vector<VkVertexInputBindingDescription> vk_vertex_binding_desc;

        for (auto& desc : vertex_binding_desc)
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

        VkPipelineCreateFlags2CreateInfoKHR const flags_2
        {
            .sType =  VK_STRUCTURE_TYPE_PIPELINE_CREATE_FLAGS_2_CREATE_INFO,
            .pNext = &pipeline_rendering,
            .flags = VK_PIPELINE_CREATE_2_DESCRIPTOR_HEAP_BIT_EXT
        };

        //VK_PIPELINE_CREATE_2_DESCRIPTOR_HEAP_BIT_EXT
        // If VkPipelineCreateFlags2CreateInfoKHR::flags does not include VK_PIPELINE_CREATE_2_DESCRIPTOR_HEAP_BIT_EXT
        VkGraphicsPipelineCreateInfo const pipeline_info
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &flags_2,
            .flags = 0,
            .stageCount = static_cast<uint32_t>(shader_stages.size()),
            .pStages = shader_stages.data(),
            .pVertexInputState = &vertex_input_info,
            .pInputAssemblyState = &input_assembly,
            .pTessellationState = nullptr,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterizer_state,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &color_blend,
            .pDynamicState = &dynamic_state_info,
            .layout = VK_NULL_HANDLE, // m_pipeline_layout->Handle(),
            .renderPass = nullptr,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0
        };

        
        return std::make_unique<Pipeline>(vk_device, pipeline_info);
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
            case Gfx::VertexAttribute::Local0:
            case Gfx::VertexAttribute::Local1:
            case Gfx::VertexAttribute::Local2:
            case Gfx::VertexAttribute::Local3:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            case Gfx::VertexAttribute::World0:
            case Gfx::VertexAttribute::World1:
            case Gfx::VertexAttribute::World2:
            case Gfx::VertexAttribute::World3:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
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
            case Gfx::VertexAttribute::Local0:
                return 3;
            case Gfx::VertexAttribute::Local1:
                return 4;
            case Gfx::VertexAttribute::Local2:
                return 5;
            case Gfx::VertexAttribute::Local3:
                return 6;
            case Gfx::VertexAttribute::World0:
                return 7;
            case Gfx::VertexAttribute::World1:
                return 8;
            case Gfx::VertexAttribute::World2:
                return 9;
            case Gfx::VertexAttribute::World3:
                return 10;
        }

        std::unreachable();
    }

    static uint32_t GetBinding(Gfx::VertexBinding binding)
    {
        switch (binding)
        {
            case Gfx::VertexBinding::PerVertex:
                return 0;
            case Gfx::VertexBinding::PerInstance:
                return 1;
        }

        std::unreachable();
    }

    void PipelineFactory::SetVertexBinding(Gfx::VertexBinding binding, std::size_t stride)
    {
        if (binding == Gfx::VertexBinding::PerVertex)
        {
            vertex_binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            vertex_binding_desc[0].binding = 0;
            vertex_binding_desc[0].stride = stride;
            return;
        }
        if (binding == Gfx::VertexBinding::PerInstance)
        {
            vertex_binding_desc[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            vertex_binding_desc[1].binding = 1;
            vertex_binding_desc[1].stride = stride;
            return;
        }
        std::unreachable();
    }

    void PipelineFactory::SetVertexAttributes(std::vector<std::span<Gfx::VertexDescription const>> const& attributes)
    {
        vertex_arrtibutes.clear();
        vertex_arrtibutes.reserve(attributes.size());

        for (auto const& subset : attributes)
        {
            for (auto const& attribute : subset)
            {
                vertex_arrtibutes.push_back({
                    .location = GetLocation(attribute.attribute),
                    .binding = GetBinding(attribute.binding),
                    .format = GetFormat(attribute.attribute),
                    .offset = attribute.offset
                });
            }
        }
    }

} // Rc::Render
