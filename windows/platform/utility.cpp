#include "utility.h"
#include "base/utf.h"
#include <string>
#include <string_view>

namespace Rc::Windows
{
    std::string WcharToUtf8(const WCHAR* wstr)
    {
        return Utf8::FromUtf16({reinterpret_cast<char16_t const*>(wstr)});
    }

    // std::wstring Utf8ToWchar(std::string_view str)
    // {
    // }
    
} // Rc::Windows