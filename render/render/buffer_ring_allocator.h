#pragma once

#include "buffer.h"
#include <memory>
#include <vector>

namespace Rc::Render
{
    //
    // Ring buffer allocator of fixed size chunks.
    //
    class BufferRingAllocator
    {
    public:
        static constexpr uint64_t default_chunk_size = 4'194'304;

        BufferRingAllocator() = default;
        ~BufferRingAllocator() = default;

        BufferRingAllocator(std::unique_ptr<Buffer> buffer, uint64_t chunk_size);

        BufferRingAllocator(BufferRingAllocator const&) = delete;
        BufferRingAllocator& operator=(BufferRingAllocator const&) = delete;
        BufferRingAllocator(BufferRingAllocator&&) = default;
        BufferRingAllocator& operator=(BufferRingAllocator&&) = default;

        std::optional<BufferRegion> Allocate(uint64_t required);

        void Complete(uint64_t timeline)
        {
            m_timeline_complete = timeline;
        }

        uint64_t TimelineValue() const
        {
            return m_timeline_allocate;
        }

        uint64_t ChunkSize() const
        {
            return m_chunk_size;
        }

        uint64_t ChunkCount() const
        {
            return m_chunks.size();
        }

        // Get number of available chunks.
        uint64_t ChunkAvailable(uint64_t timeline_complete) const;

        Buffer& GetBuffer() //------------------------------ TODO: Review potencialni dira!
        {
            return *m_buffer;
        }

        void Reset();

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
        uint64_t m_chunk_size {0};

        struct Chunk
        {
            uint64_t offset {0};
            uint64_t timeline {0};
        };

        std::vector<Chunk> m_chunks;

        uint64_t m_timeline_complete {0};
        uint64_t m_timeline_allocate {0};

        std::size_t m_position {0};
    };

} // Rc::Render