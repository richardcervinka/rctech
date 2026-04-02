#pragma once

#include <string>
#include <string_view>
#include <Windows.h>
#include <format>

namespace Rc::Log
{
    void Debug(std::string_view str)
    {
        OutputDebugString(str.data());
    }
    
} // namespce Rc::Log