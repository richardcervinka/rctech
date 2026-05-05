#pragma once

#include <string>
#include <Windows.h>
#include "base/utf.h"

namespace Rc::Windows
{
    std::string WcharToString(const WCHAR* wstr);

    inline std::wstring ToWString(std::u16string_view str)
    {
        return std::wstring(str.begin(), str.end());
    }

    inline std::wstring ToWString(std::u32string_view str)
    {
        const auto u16 = Utf16::FromUtf32(str);
        return {u16.begin(), u16.end()};
    }

} // Rc::Windows