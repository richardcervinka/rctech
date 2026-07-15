#include "error.h"
#include "utility.h"

namespace Rc
{
    const char* SystemException::what() const
    {
        LPWSTR message = nullptr;

        DWORD size = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            hresult,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPWSTR>(&message),
            0,
            nullptr
        );

        buffer = Windows::WcharToUtf8(message);
        LocalFree(message);
        return buffer.c_str();
    }

} // Rc