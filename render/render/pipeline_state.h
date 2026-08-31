#pragma once

#include "vulkan/device.h"
#include <memory>
#include <span>
#include <vector>
#include <array>
#include "core/vertex.h"
#include "texture.h"
#include "shader.h"

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

        VkPipeline Underlying() const { return vk_pipeline; }

    private:
        VulkanDevice const& vk_device;
        VkPipeline vk_pipeline {VK_NULL_HANDLE};
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

        VkPipelineLayout Underlying() const { return vk_pipeline_layout; }

    private:
        VulkanDevice const& vk_device;
        VkDescriptorSetLayout vk_descriptor_set_layout {VK_NULL_HANDLE};
        VkPipelineLayout vk_pipeline_layout {VK_NULL_HANDLE};
    };

    // Setup vertex buffer:
    // 1. SetVertexInputBinding
    // 2. SetVertexInputAttributes
    //
    class PipelineFactory
    {
    public:
        PipelineFactory() = default;

        PipelineFactory(VulkanDevice const& vk_device);

        void SetOutputFormat(PixelFormat format);

        void SetVertexShader(Shader const& vs)
        {
            this->vs = vs.Underlying();
        }

        void SetPixelShader(Shader const& ps)
        {
            this->ps = ps.Underlying();
        }

        void SetPipelineLayout(std::shared_ptr<PipelineLayout> layout)
        {
            pipeline_layout = std::move(layout);
        }

        //void SetVertexBinding()

        // stride = size of vertex
        // 0 to disable vertex input binding
        void SetVertexAttributes(std::vector<std::span<Gfx::VertexDescription const>> const& attributes);
        //void SetInstanceInput(std::span<Gfx::VertexDescription const> attributes);

        void SetVertexBinding(Gfx::VertexBinding binding, std::size_t stride);

        // TODO: Do not return raw Vk object
        std::unique_ptr<Pipeline> Create();
        
    private:
        static constexpr std::size_t color_attachment_count = 4;

        std::array<VkFormat, color_attachment_count> color_attachment_formats
        {
            // RenderTargetSlot::FrameBuffer
            VK_FORMAT_UNDEFINED, // VK_FORMAT_R8G8B8A8_SRGB, //---------------------------------------------------------------- Swap Chain
            //
            VK_FORMAT_UNDEFINED,
            //
            VK_FORMAT_UNDEFINED,
            //
            VK_FORMAT_UNDEFINED
        };

        VulkanDevice const& vk_device;
        VkShaderModule vs {nullptr};
        VkShaderModule ps {nullptr};
        std::vector<VkVertexInputAttributeDescription> vertex_arrtibutes;
        std::array<VkVertexInputBindingDescription, 2> vertex_binding_desc {};
        VkPipelineInputAssemblyStateCreateInfo input_assembly {};
        VkPipelineViewportStateCreateInfo viewport_state {};
        VkPipelineRasterizationStateCreateInfo rasterizer_state {};
        VkPipelineMultisampleStateCreateInfo multisampling {};
        std::array<VkPipelineColorBlendAttachmentState, color_attachment_count> color_blend_attachments {};
        VkPipelineColorBlendStateCreateInfo color_blend {};
        VkPipelineRenderingCreateInfo pipeline_rendering {};
        std::shared_ptr<PipelineLayout> pipeline_layout;
    };

} // Rc::Render