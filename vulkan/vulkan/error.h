#pragma once

#include <vulkan/vulkan.h>
#include "base/error.h"
#include <source_location>

namespace Rc
{
    std::string VkResultToString(VkResult value);

    class VulkanException : public Exception
    {
    public:
        explicit VulkanException(VkResult r, std::source_location src = std::source_location::current()) :
            result{r},
            src{src}
        {}

        char const* what() const noexcept override;

        VkResult Value() const { return result; }

        // Convert the VkResult value to string.
        std::string ValueStr() const;

    private:
        VkResult result;
        std::source_location src;
    };

    class VulkanLoaderException : public Exception
    {
    public:
        VulkanLoaderException(char const* fn) : fn{fn} {}

        char const* what() const override
        {
            buffer = "NULL ";
            buffer += fn;
            return buffer.c_str();
        }

    private:
        char const* fn;
    };

} // Rc