#pragma once

#include <exception>
#include <string>

namespace Rc
{
    class Exception : public std::exception
    {
    public:
        Exception() = default;
        ~Exception() override = default;

        Exception([[maybe_unused]] Exception const& other) {}
        Exception& operator=([[maybe_unused]] Exception const& other) { return *this; }
        Exception([[maybe_unused]] Exception&& other) noexcept {}
        Exception& operator=([[maybe_unused]] Exception&& other) noexcept { return *this; }

    protected:
        // Non-copyable buffer for the what() method.
        mutable std::string m_buffer;
    };

} // Rc