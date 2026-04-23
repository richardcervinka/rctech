#pragma once

#include <vulkan/vulkan.h>
#include "base/error.h"

namespace Rc
{
    class VulkanException : public Exception
    {
    public:
        VulkanException(VkResult r) : m_result{r} {}

        char const* what() const override
        {
            m_buffer = "Vulkan error (VulkanError::what() NOT IMPLEMENTED)";
            return m_buffer.c_str();
        }

        VkResult Value() const { return m_result; }

    private:
        VkResult m_result;
    };

    class VulkanLoaderException : public Exception
    {
    public:
        VulkanLoaderException(char const* fn) : m_fn{fn} {}

        char const* what() const override
        {
            m_buffer = "NULL ";
            m_buffer += m_fn;
            return m_buffer.c_str();
        }

    private:
        char const* m_fn;
    };

} // Rc