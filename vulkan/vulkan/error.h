#pragma once

#include <vulkan/vulkan.h>
#include "base/error.h"
#include <source_location>

namespace Rc
{
    class VulkanException : public Exception
    {
    public:
        explicit VulkanException(VkResult r, std::source_location src = std::source_location::current()) :
            m_result{r},
            m_src{src}
        {}

        char const* what() const noexcept override
        {
            m_buffer = m_src.function_name();
            m_buffer += " error: ";
            // m_buffer +=
            return m_buffer.c_str();
        }

        VkResult Value() const { return m_result; }

    private:
        VkResult m_result;
        std::source_location m_src;
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