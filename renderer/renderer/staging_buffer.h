#pragma once

#include "buffer.h"
#include <memory>
#include <span>
#include <cstddef>

namespace Rc
{
    class StagingBuffer : public Buffer
    {
    public:
        StagingBuffer(VmaAllocator vma_allocator, std::size_t size);

        ~StagingBuffer() = default;

        StagingBuffer(const StagingBuffer&) = delete;
        StagingBuffer& operator=(const StagingBuffer&) = delete;
        StagingBuffer(StagingBuffer&& other) = delete;
        StagingBuffer& operator=(StagingBuffer&& other) = delete;

        std::span<std::byte> Data();
        //std::span<std::byte> Map(); // -------------- unmap?
    };

} // Rc