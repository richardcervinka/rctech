#pragma once

#include <string>
#include <Windows.h>

namespace Rc::Windows
{
    std::string WcharToUtf8(const WCHAR* wstr);

} // Rc::Windows