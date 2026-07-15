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
            buffer{std::move(buffer)}
        {
            assert(this->buffer != nullptr);
        }

        BufferLinearAllocator(BufferLinearAllocator const&) = delete;
        BufferLinearAllocator& operator=(BufferLinearAllocator const&) = delete;
        BufferLinearAllocator(BufferLinearAllocator&&) = default;
        BufferLinearAllocator& operator=(BufferLinearAllocator&&) = default;

        BufferRegion Allocate(uint64_t size)
        {
            auto region = buffer->GetRegion(offset, size);
            offset += size;
            return region;
        }

        uint64_t Capacity() const
        {
            return buffer->Size();
        }

        uint64_t Available() const
        {
            return buffer->Size() - offset;
        }

        Buffer& GetBuffer() //------------------------------ potencialni dira!
        {
            return *buffer;
        }

        Buffer const& GetBuffer() const
        {
            return *buffer;
        }

        void Reset()
        {
            offset = 0;
        }

        template<typename T>
        std::span<T> Map(BufferRegion const& region)
        {
            auto raw = buffer->Map(region);

            return {
                reinterpret_cast<T*>(raw.data()),
                region.Size() / sizeof(T)
            };
        }

    private:
        std::unique_ptr<Buffer> buffer;
        uint64_t offset {0};
        //uint64_t m_generation {0};
    };

} // Rc::Render