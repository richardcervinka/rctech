#pragma once

#include "buffer.h"
#include <span>
#include <cstddef>
#include <cassert>

namespace Rc::Render
{
    class StagingBuffer
    {
    public:
        StagingBuffer() = default;
        
        explicit StagingBuffer(std::unique_ptr<Buffer> buffer) :
            m_buffer{std::move(buffer)}
        {}

        ~StagingBuffer() = default;

        StagingBuffer(const StagingBuffer&) = delete;
        StagingBuffer& operator=(const StagingBuffer&) = delete;
        StagingBuffer(StagingBuffer&& other) = delete;
        StagingBuffer& operator=(StagingBuffer&& other) = delete;

        uint64_t Size() const
        {
            return m_buffer->Size();
        }

        std::span<std::byte> Data()
        {
            return m_buffer->Data();
        }

        Buffer& GetBuffer()
        {
            assert(m_buffer != nullptr);
            return *m_buffer;
        }

    private:
        friend class CommandBuffer;

        std::unique_ptr<Buffer> m_buffer;
    };

} // Rc::Render