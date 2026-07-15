#pragma once

#include <Windows.h>
#include "base/error.h"

namespace Rc
{
    class SystemException : public Exception
    {
    public:
        SystemException(HRESULT h) : hresult{h} {}
        SystemException(DWORD e) : hresult{HRESULT_FROM_WIN32(e)} {}

        const char* what() const override;

        uint32_t Code() const { return hresult; }

    private:
        HRESULT hresult;
    };

} // Rc