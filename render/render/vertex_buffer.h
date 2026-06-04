#pragma once

#include "buffer.h"
#include <memory>
#include <span>
#include <cstddef>

namespace Rc::Render
{
    class VertexBuffer : public Buffer
    {
    public:
        VertexBuffer(VmaAllocator vma_allocator, std::size_t size);
    };

} // Rc::Render
