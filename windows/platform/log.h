#pragma once

#include <string>
#include <string_view>
#include <Windows.h>
#include <format>
#include "base/utf.h"

namespace Rc::Log
{
    void Debug(std::string_view str)
    {
        const auto ustr = Utf16::FromUtf8(str);
        OutputDebugString(reinterpret_cast<LPCWSTR>(ustr.data()));
    }

    void Error(std::string_view str)
    {
        const auto ustr = Utf16::FromUtf8(str);
        OutputDebugString(reinterpret_cast<LPCWSTR>(ustr.data()));
    }
    
} // namespce Rc::Log