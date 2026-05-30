#pragma once

#include <string>
#include <Windows.h>

namespace Rc::Log
{
    inline void Debug(std::string str)
    {
        str += '\n';
        OutputDebugStringA(reinterpret_cast<LPCSTR>(str.data()));
    }

    inline void Error(std::string str)
    {
        str += '\n';
        OutputDebugStringA(reinterpret_cast<LPCSTR>(str.data()));
    }
    
} // namespce Rc::Log