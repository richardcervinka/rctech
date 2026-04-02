#include "utility.h"

namespace Windows
{
    std::string WcharToString(const WCHAR* wstr)
    {
        int const size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
        if (size < 1)
        {
            return {};
        }
        std::string str(size - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str.data(), size, nullptr, nullptr);
        return str;
    }
}