#pragma once

#include <Windows.h>
#include "base/error.h"

namespace Rc
{
    class SystemError : public Error
    {
    public:
        SystemError(HRESULT h) : m_hresult{h} {}
        SystemError(DWORD e) : m_hresult{HRESULT_FROM_WIN32(e)} {}

        const char* what() const override;

        uint32_t Code() const { return m_hresult; }

    private:
        HRESULT m_hresult;
    };

} // Rc