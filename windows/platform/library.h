#pragma once

#include <Windows.h>
#include <string>

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
        T GetPfn(std::string const& fn_name)
        {
            return reinterpret_cast<T>(GetProcAddress(hmodule, fn_name.c_str()));
        }

    private:
        std::string name;

        HMODULE hmodule {NULL};
    };

} // Rc {