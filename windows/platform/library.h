#pragma once

#include <Windows.h>
#include <string>
#include <string_view>

namespace Rc
{
    class Library
    {
    public: 
        Library() = default;

        Library(Library const&) = delete;
        Library& operator=(Library const&) = delete;

        Library(Library&& other) noexcept;
        Library& operator=(Library&& other) noexcept;

        ~Library();

        explicit Library(std::string name);

        template<typename T>
        T GetPfn(std::string const& name)
        {
            return reinterpret_cast<T>(GetProcAddress(m_hmodule, name.c_str()));
        }

    private:
        std::string m_name;

        HMODULE m_hmodule {NULL};
    };

} // Rc {