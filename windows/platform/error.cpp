#include "error.h"
#include "windows/utility.h"

namespace Rc
{
    const char* SystemError::what() const
    {
        LPWSTR buffer = nullptr;

        DWORD size = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            m_hresult,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&buffer,
            0,
            nullptr
        );

        m_buffer = Windows::WcharToString(buffer);

        LocalFree(buffer);

        return m_buffer.c_str();

    }

} // Rc