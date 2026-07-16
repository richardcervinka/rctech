#include "shader.h"

namespace Rc::Render
{
    Shader::Shader(VulkanDevice const& vk_device, std::span<uint32_t const> spirv) : vk_device{vk_device}
    {
        VkShaderModuleCreateInfo const create_info
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = spirv.size() * sizeof(uint32_t),
            .pCode = spirv.data()
        };

        vk_shader_module = vk_device.CreateShaderModule(create_info);
    }

    Shader::~Shader()
    {
        vk_device.DestroyShaderModule(vk_shader_module);
    }

} // Rc::Render