#include "library.h"
#include "error.h"
#include "base/utf.h"

namespace Rc
{
    Library::Library(std::string name) : m_name{std::move(name)}
    {
        const auto lname = Utf16::FromUtf8(m_name);
        m_hmodule = LoadLibrary(reinterpret_cast<LPCWSTR>(lname.c_str()));

        if (m_hmodule == NULL)
        {
            throw SystemException(GetLastError());
        }
    }

    Library::Library(Library&& other) noexcept :
        m_hmodule{other.m_hmodule},
        m_name{std::move(other.m_name)}
    {
        other.m_hmodule = NULL;
    }

    Library& Library::operator=(Library&& other) noexcept
    {
        std::swap(m_hmodule, other.m_hmodule);
        std::swap(m_name, other.m_name);
        return *this;
    }

    Library::~Library()
    {
        if (m_hmodule != NULL)
        {
            FreeLibrary(m_hmodule);
        }
    }

} // Rc