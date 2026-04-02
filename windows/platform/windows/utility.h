#pragma once

#include <string>
#include <Windows.h>

namespace Windows
{
    std::string WcharToString(const WCHAR* wstr);
}