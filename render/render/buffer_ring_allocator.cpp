#include "buffer_ring_allocator.h"

namespace Rc::Render
{
    BufferRingAllocator::BufferRingAllocator(std::unique_ptr<Buffer> buffer, uint64_t chunk_size) :
        m_buffer{std::move(buffer)},
        m_chunk_size{chunk_size}
    {
        assert(m_buffer != nullptr);

        uint64_t chunk_count = m_buffer->Size() / chunk_size;

        m_chunks.reserve(chunk_count);

        for (int i = 0; i < chunk_count; i++)
        {
            m_chunks.push_back({.offset = i * chunk_size});
        }
    }

    std::optional<BufferRegion> BufferRingAllocator::Allocate(uint64_t required)
    {
        if (required > m_chunk_size)
        {
            return std::nullopt;
        }

        auto position = m_position;

        for (std::size_t i = 0; i < m_chunks.size(); i++)
        {
            auto& chunk = m_chunks[position];

            if (chunk.timeline <= m_timeline_complete)
            {
                // Next allocating will start at the next position.
                m_position = (position + 1) % m_chunks.size();

                chunk.timeline = (++m_timeline_allocate);

                return m_buffer->GetRegion(chunk.offset, required);
            }

            position = (position + 1) % m_chunks.size();
        }

        return std::nullopt;
    }

    // Get number of available chunks.
    uint64_t BufferRingAllocator::ChunkAvailable(uint64_t timeline_complete) const
    {
        uint64_t result = 0;

        for (auto& chunk : m_chunks)
        {
            if (chunk.timeline <= timeline_complete)
            {
                result++;
            }
        }

        return result;
    }

    void BufferRingAllocator::Reset()
    {
        m_timeline_complete = 0;
        m_timeline_allocate = 0;
        m_position = 0;

        for (auto& chunk : m_chunks)
        {
            chunk.timeline = 0;
        }
    }

} // Rc::Render