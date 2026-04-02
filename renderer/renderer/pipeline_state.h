#pragma once

#include "vulkan/device.h"
#include <memory>
#include <span>
#include <optional>
#include <vector>

namespace Rc
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
        VkPipelineLayout m_vk_pipeline_layout {VK_NULL_HANDLE};
    };

    class PipelineFactory
    {
    public:
        PipelineFactory() = default;

        explicit PipelineFactory(VulkanDevice const& vk_device);

        void SetVertexShader(VkShaderModule vs)
        {
            m_vk_vs = std::move(vs);
        }

        void SetPixelShader(VkShaderModule ps)
        {
            m_vk_ps = std::move(ps);
        }

        void SetVertexInputAttributes(std::span<VkVertexInputAttributeDescription const> attributes)
        {
            m_vk_vertex_input_arrtibutes = {attributes.begin(), attributes.end()};
        }

        void SetVertexInputRate(uint32_t stride)
        {
            m_vk_vertex_binding_desc = VkVertexInputBindingDescription(0, stride, VK_VERTEX_INPUT_RATE_VERTEX);
        }

        void SetPipelineLayout(std::shared_ptr<PipelineLayout> layout) { m_vk_pipeline_layout = std::move(layout); }

        // stride = vertex size
        // 0 to disable vertex input binding
        void SetVertexInputBinding(std::size_t stride);

        // TODO: Do not return raw Vk object
        std::unique_ptr<Pipeline> Create();
        
    private:
        VulkanDevice const* m_vk_device {nullptr};
        VkShaderModule m_vk_vs {nullptr};
        VkShaderModule m_vk_ps {nullptr};
        VkFormat m_vk_format {VK_FORMAT_R8G8B8A8_SRGB}; // ------------------------------ UNDEFINED
        std::vector<VkVertexInputAttributeDescription> m_vk_vertex_input_arrtibutes;
        VkVertexInputBindingDescription m_vk_vertex_binding_desc {};
        VkPipelineInputAssemblyStateCreateInfo m_vk_input_assembly {};
        VkPipelineViewportStateCreateInfo m_vk_viewport_state {};
        VkPipelineRasterizationStateCreateInfo m_vk_rasterizer_state {};
        VkPipelineMultisampleStateCreateInfo m_vk_multisampling {};
        VkPipelineColorBlendAttachmentState m_vk_color_blend_attachment {};
        VkPipelineColorBlendStateCreateInfo m_vk_color_blend {};
        VkPipelineRenderingCreateInfo m_vk_pipeline_rendering {};
        std::shared_ptr<PipelineLayout> m_vk_pipeline_layout;
    };

} // Rc