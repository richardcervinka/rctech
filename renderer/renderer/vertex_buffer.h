#pragma once

#include "buffer.h"
#include <memory>
#include <span>
#include <cstddef>

namespace Rc
{
    class VertexBuffer : public Buffer
    {
    public:
        VertexBuffer(VmaAllocator vma_allocator, std::size_t size);

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&& other) = delete;
        VertexBuffer& operator=(VertexBuffer&& other) = delete;

        ~VertexBuffer() = default;

        uint64_t Size() const { return m_vma_allocation_info.size; }
    };

} // Rc
