#pragma once

#include "vulkan/device.h"
#include <memory>
#include <span>
#include <vector>
#include <array>
#include "core/vertex.h"

namespace Rc::Render
{
    class Pipeline
    {
    public:
        Pipeline(VulkanDevice const& vk_device, VkGraphicsPipelineCreateInfo const& create_info);

        ~Pipeline();

        Pipeline(Pipeline const&) = delete;
        Pipeline& operator=(Pipeline const&) = delete;
        Pipeline(Pipeline&& other) = delete;
        Pipeline& operator=(Pipeline&& other) = delete;

        VkPipeline Handle() const { return m_vk_pipeline; }

    private:
        VulkanDevice const* m_vk_device {nullptr};
        VkPipeline m_vk_pipeline {VK_NULL_HANDLE};
    };

    class PipelineLayout
    {
    public:
        explicit PipelineLayout(VulkanDevice const& vk_device);

        ~PipelineLayout();

        PipelineLayout(PipelineLayout const&) = delete;
        PipelineLayout& operator=(PipelineLayout const&) = delete;
        PipelineLayout(PipelineLayout&& other) = delete;
        PipelineLayout& operator=(PipelineLayout&& other) = delete;

        VkPipelineLayout Handle() const { return m_vk_pipeline_layout; }

    private:
        VulkanDevice const* m_vk_device {nullptr};
        VkDescriptorSetLayout m_vk_descriptor_set_layout {VK_NULL_HANDLE};
        VkPipelineLayout m_vk_pipeline_layout {VK_NULL_HANDLE};
    };

    // Setup vertex buffer:
    // 1. SetVertexInputBinding
    // 2. SetVertexInputAttributes
    //
    class PipelineFactory
    {
    public:
        PipelineFactory() = default;

        explicit PipelineFactory(VulkanDevice const& vk_device);

        void SetVertexShader(VkShaderModule vs)
        {
            m_vs = vs;
        }

        void SetPixelShader(VkShaderModule ps)
        {
            m_ps = ps;
        }

        void SetPipelineLayout(std::shared_ptr<PipelineLayout> layout)
        {
            m_pipeline_layout = std::move(layout);
        }

        // stride = size of vertex
        // 0 to disable vertex input binding
        void SetVertexInput(std::size_t stride, std::span<Gfx::VertexDescription const> attributes);
        void SetInstanceInput(std::size_t stride, std::span<Gfx::VertexDescription const> attributes);

        // TODO: Do not return raw Vk object
        std::unique_ptr<Pipeline> Create();
        
    private:
        VulkanDevice const* m_device {nullptr};
        VkShaderModule m_vs {nullptr};
        VkShaderModule m_ps {nullptr};
        VkFormat m_format {VK_FORMAT_R8G8B8A8_SRGB}; // ------------------------------ UNDEFINED
        std::vector<VkVertexInputAttributeDescription> m_vertex_arrtibutes;
        std::vector<VkVertexInputAttributeDescription> m_instance_arrtibutes;
        std::array<VkVertexInputBindingDescription, 2> m_vertex_binding_desc {};
        VkPipelineInputAssemblyStateCreateInfo m_input_assembly {};
        VkPipelineViewportStateCreateInfo m_viewport_state {};
        VkPipelineRasterizationStateCreateInfo m_rasterizer_state {};
        VkPipelineMultisampleStateCreateInfo m_multisampling {};
        VkPipelineColorBlendAttachmentState m_color_blend_attachment {};
        VkPipelineColorBlendStateCreateInfo m_color_blend {};
        VkPipelineRenderingCreateInfo m_pipeline_rendering {};
        std::shared_ptr<PipelineLayout> m_pipeline_layout;
    };

} // Rc::Render