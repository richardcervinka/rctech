#include "error.h"
#include "utility.h"

namespace Rc
{
    const char* SystemException::what() const
    {
        LPWSTR buffer = nullptr;

        DWORD size = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            m_hresult,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPWSTR>(&buffer),
            0,
            nullptr
        );

        m_buffer = Windows::WcharToUtf8(buffer);
        LocalFree(buffer);
        return m_buffer.c_str();
    }

} // Rc