#pragma once

#include <exception>
#include <string>

namespace Rc
{
    class Error : public std::exception
    {
    public:
        Error() = default;
        ~Error() override = default;

        Error(Error const&) {}
        Error& operator=(Error const&) { return *this; }
        Error(Error&&) {}
        Error& operator=(Error&&) { return *this; }

    protected:
        // Non-copyable buffer for the what() method.
        mutable std::string m_buffer;
    };

} // Rc