#include "system.h"

namespace Rc
{
    int System::GetCpuThreads() const
    {
        SYSTEM_INFO si;
        ::GetSystemInfo(&si);
        return static_cast<int>(si.dwNumberOfProcessors);
    }

} // Rc