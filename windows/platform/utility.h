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

} // Rc::Windows