#pragma once

#include <string>
#include <Windows.h>
#include "base/utf.h"

namespace Rc::Windows
{
    std::string WcharToString(const WCHAR* wstr);

} // Rc::Windows