#pragma once

#include "buffer.h"
#include <cstddef>
#include <cstdint>

namespace Rc::Render
{
    enum class IndexType
    {
        Uint16,
        Uint32
    };

    struct IndexBufferView
    {
        uint64_t offset;
        uint64_t size;
        IndexType type;
    };

    struct IndexBufferInfo
    {
        uint64_t size;
        IndexType type;
    };

    
    // Static index buffer
    class IndexBuffer : public Buffer
    {
    public:
        IndexBuffer(VmaAllocator vma_allocator, uint64_t size);
    };

} // Rc::Render
