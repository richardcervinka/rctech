#include "library.h"
#include "error.h"
#include "base/utf.h"

namespace Rc
{
    Library::Library(std::string name) : name{std::move(name)}
    {
        const auto lname = Utf16::FromUtf8(this->name);
        hmodule = LoadLibrary(reinterpret_cast<LPCWSTR>(lname.c_str()));

        if (hmodule == NULL)
        {
            throw SystemException(GetLastError());
        }
    }

    Library::Library(Library&& other) noexcept :
        hmodule{other.hmodule},
        name{std::move(other.name)}
    {
        other.hmodule = NULL;
    }

    Library& Library::operator=(Library&& other) noexcept
    {
        std::swap(hmodule, other.hmodule);
        std::swap(name, other.name);
        return *this;
    }

    Library::~Library()
    {
        if (hmodule != NULL)
        {
            FreeLibrary(hmodule);
        }
    }

} // Rc