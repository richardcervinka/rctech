#pragma once

#include "buffer.h"
#include <memory>

namespace Rc::Render
{
    class BufferPool //---------------- Prejmenovat na BufferPoolAllocator
    {
    public:

    private:
        std::unique_ptr<Buffer> m_buffer;
    };

} // Rc::Render