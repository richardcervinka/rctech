#include "system.h"
#include <Windows.h>

namespace Rc::System
{
    int GetCpuThreads()
    {
        SYSTEM_INFO si;
        ::GetSystemInfo(&si);
        return static_cast<int>(si.dwNumberOfProcessors);
    }

} // Rc