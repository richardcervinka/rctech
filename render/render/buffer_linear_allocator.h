#pragma once

#include "buffer.h"
#include <memory>

namespace Rc::Render
{
    //
    // Linear buffer allocator.
    //
    class BufferLinearAllocator
    {
    public:
        BufferLinearAllocator() = default;
        ~BufferLinearAllocator() = default;

        explicit BufferLinearAllocator(std::unique_ptr<Buffer> buffer) :
            m_buffer{std::move(buffer)}
        {
            assert(m_buffer != nullptr);
        }

        BufferLinearAllocator(BufferLinearAllocator const&) = delete;
        BufferLinearAllocator& operator=(BufferLinearAllocator const&) = delete;
        BufferLinearAllocator(BufferLinearAllocator&&) = default;
        BufferLinearAllocator& operator=(BufferLinearAllocator&&) = default;

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

        Buffer& GetBuffer() //------------------------------ potencialni dira!
        {
            return *m_buffer;
        }

        void Reset()
        {
            m_offset = 0;
        }

        template<typename T>
        std::span<T> Map(BufferRegion const& region)
        {
            auto raw = m_buffer->Map(region);

            return {
                reinterpret_cast<T*>(raw.data()),
                region.Size() / sizeof(T)
            };
        }

    private:
        std::unique_ptr<Buffer> m_buffer;
        uint64_t m_offset {0};
        //uint64_t m_generation {0};
    };

} // Rc::Render