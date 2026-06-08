#pragma once

#include "buffer.h"
#include <memory>

namespace Rc::Render
{
    class BufferLinear
    {
    public:
        BufferLinear() = default;
        ~BufferLinear() = default;

        explicit BufferLinear(std::unique_ptr<Buffer> buffer) :
            m_buffer{std::move(buffer)}
        {
            assert(m_buffer != nullptr);
        }

        BufferLinear(BufferLinear const&) = delete;
        BufferLinear& operator=(BufferLinear const&) = delete;
        BufferLinear(BufferLinear&&) = default;
        BufferLinear& operator=(BufferLinear&&) = default;

        BufferRegion Allocate(uint64_t size)
        {
            auto region = m_buffer->GetRegion(m_offset, size);
            m_offset += size;
            return region;
        }

        uint64_t Capacity() const
        {
            return m_buffer->Size();
        }

        uint64_t Available() const
        {
            return m_buffer->Size() - m_offset;
        }

        Buffer& GetBuffer()
        {
            return *m_buffer;
        }

    private:
        std::unique_ptr<Buffer> m_buffer;
        uint64_t m_offset {0};
    };

} // Rc::Render