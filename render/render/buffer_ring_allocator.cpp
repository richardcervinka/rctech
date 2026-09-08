#include "buffer_ring_allocator.h"

namespace Rc::Render
{
    BufferRingAllocator::BufferRingAllocator(std::unique_ptr<Buffer> buffer, uint64_t chunk_size) :
        buffer{std::move(buffer)},
        chunk_size{chunk_size}
    {
        assert(this->buffer != nullptr);

        uint64_t chunk_count = this->buffer->Size() / chunk_size;

        chunks.reserve(chunk_count);

        for (int i = 0; i < chunk_count; i++)
        {
            chunks.push_back({.offset = i * chunk_size});
        }
    }

    std::optional<BufferRegion> BufferRingAllocator::Allocate(uint64_t required) // ------- throw exception? std::error?
    {
        if (required > chunk_size)
        {
            return std::nullopt;
        }

        auto index = position;

        for (std::size_t i = 0; i < chunks.size(); i++)
        {
            auto& chunk = chunks[index];

            if (chunk.timeline <= timeline_complete)
            {
                // Next allocating will start at the next position.
                position = (index + 1) % chunks.size();

                chunk.timeline = (++timeline_allocate);

                return buffer->GetRegion(chunk.offset, required);
            }

            index = (index + 1) % chunks.size();
        }

        return std::nullopt;
    }

    // Get number of available chunks.
    uint64_t BufferRingAllocator::ChunkAvailable(uint64_t timeline_complete) const
    {
        uint64_t result = 0;

        for (auto& chunk : chunks)
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
        timeline_complete = 0;
        timeline_allocate = 0;
        position = 0;

        for (auto& chunk : chunks)
        {
            chunk.timeline = 0;
        }
    }

} // Rc::Render