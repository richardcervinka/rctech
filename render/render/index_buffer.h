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
    
    class IndexBuffer : public Buffer
    {
    public:
        IndexBuffer(VmaAllocator vma_allocator, std::size_t size);

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;
        IndexBuffer(IndexBuffer&& other) = delete;
        IndexBuffer& operator=(IndexBuffer&& other) = delete;

        ~IndexBuffer() = default;
    };

    // class IndexBufferView
    // {
    // private:
    //     int m_slot;
    //     uint64_t m_offset;
    //     uint64_t m_size;
    //     IndexType m_type;
    // };

} // Rc::Render
