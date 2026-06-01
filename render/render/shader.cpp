#include "shader.h"

namespace Rc::Render
{
    Shader::Shader(VulkanDevice const& vk_device, std::span<uint32_t const> spirv) :
        m_vk_device{&vk_device}
    {
        VkShaderModuleCreateInfo const create_info
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = spirv.size() * sizeof(uint32_t),
            .pCode = spirv.data()
        };

        m_vk_shader_module = m_vk_device->CreateShaderModule(create_info);
    }

    Shader::~Shader()
    {
        if (m_vk_device != nullptr)
        {
            m_vk_device->DestroyShaderModule(m_vk_shader_module);
        }
    }

} // Rc::Render