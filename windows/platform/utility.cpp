#include "utility.h"
#include "base/utf.h"
#include <string>
#include <string_view>

namespace Rc::Windows
{
    static_assert(sizeof(wchar_t) == sizeof(char16_t));

    std::string WcharToUtf8(const WCHAR* wstr)
    {
        return Utf8::FromUtf16({reinterpret_cast<char16_t const*>(wstr)});
    }
    
} // Rc::Windows