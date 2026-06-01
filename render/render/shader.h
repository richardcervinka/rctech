#pragma once

#include "vulkan/device.h"
#include <span>

namespace Rc::Render
{
    class Shader
    {
    public:
        Shader(VulkanDevice const& vk_device, std::span<uint32_t const> spirv);
        
        ~Shader();

        Shader(Shader const&) = delete;
        Shader& operator=(Shader const&) = delete;
        Shader(Shader&& other) = delete;
        Shader& operator=(Shader&& other) = delete;

        VkShaderModule Handle() const { return m_vk_shader_module; }

    private:
        VulkanDevice const* m_vk_device {nullptr};
        VkShaderModule m_vk_shader_module {VK_NULL_HANDLE};
    };

} // Rc::Render