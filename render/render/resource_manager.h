#pragma once

#include <array>
#include <memory>
#include <atomic>
#include "buffer_linear_allocator.h"
#include "buffer_ring_allocator.h"
#include "render/device.h"

namespace Rc::Render
{
    enum class ResourceFamily : uint32_t {};

    class VertexBufferHandle
    {
    public:
        VertexBufferHandle() = default;

        VertexBufferHandle(ResourceFamily family, uint64_t index, uint32_t generation) :
            m_family{family},
            m_index{index},
            m_generation{generation}
        {}

        ResourceFamily FamilyName() const
        {
            return m_family;
        }

        uint64_t Index() const
        {
            return m_index;
        }

        uint32_t Generation() const
        {
            return m_generation;
        }
        
    private:
        ResourceFamily m_family {};
        uint64_t m_index {};
        uint32_t m_generation {};
    };

    class IndexBufferHandle
    {
    public:
        IndexBufferHandle() = default;

        IndexBufferHandle(ResourceFamily family, uint64_t index, uint32_t generation) :
            m_family{family},
            m_index{index},
            m_generation{generation}
        {}

        ResourceFamily FamilyName() const
        {
            return m_family;
        }

        uint64_t Index() const
        {
            return m_index;
        }

        uint32_t Generation() const
        {
            return m_generation;
        }

    private:
        ResourceFamily m_family {};
        uint64_t m_index {};
        uint32_t m_generation {};
    };

    class InstanceBufferHandle
    {
    public:
        InstanceBufferHandle() = default;

        InstanceBufferHandle(ResourceFamily family, uint64_t index, uint32_t generation) : // ----- Private?
            m_family{family},
            m_index{index},
            m_generation{generation}
        {}

        ResourceFamily FamilyName() const
        {
            return m_family;
        }

        uint64_t Index() const
        {
            return m_index;
        }

        uint32_t Generation() const
        {
            return m_generation;
        }

    private:
        ResourceFamily m_family {};
        uint64_t m_index {};
        uint32_t m_generation {};
    };

    template<typename Handle>
    class ResourceAllocator
    {
    public:
        explicit ResourceAllocator(std::unique_ptr<Buffer> buffer) :
            allocator(std::move(buffer))
        {}

        Handle Allocate(ResourceFamily family, uint64_t size)
        {
            auto const id = count.fetch_add(1, std::memory_order_relaxed);
            regions[id] = allocator.Allocate(size);
            return {family, id, 0};
        }

        Buffer const& GetBuffer() const
        {
            return allocator.GetBuffer();
        }

        // Map handle index to a buffer region.
        std::array<BufferRegion, UINT16_MAX> regions;

    private:
        BufferLinearAllocator allocator;

        std::atomic<std::size_t> count {0};

        // TODO: Generation
    };

    struct ResourcePool
    {
        std::unique_ptr<ResourceAllocator<VertexBufferHandle>> vertex_buffer_allocator;
        std::unique_ptr<ResourceAllocator<InstanceBufferHandle>> instance_buffer_allocator;
        std::unique_ptr<ResourceAllocator<IndexBufferHandle>> index_buffer_allocator;
    };

    class ResourceManager
    {
    public:
        explicit ResourceManager(std::shared_ptr<Device> device);

        void ReserveVertexBuffer(ResourceFamily family, uint64_t capacity); // usage parameter?
        void ReserveInstanceBuffer(ResourceFamily family, uint64_t capacity); // usage parameter?
        void ReserveIndexBuffer(ResourceFamily family, uint64_t capacity); // usage parameter?

        Buffer const& GetVertexBuffer(ResourceFamily family)
        {
            return m_pools[std::to_underlying(family)].vertex_buffer_allocator->GetBuffer();
        }

        Buffer const& GetInstanceBuffer(ResourceFamily family)
        {
            return m_pools[std::to_underlying(family)].instance_buffer_allocator->GetBuffer();
        }

        Buffer const& GetIndexBuffer(ResourceFamily family)
        {
            return m_pools[std::to_underlying(family)].index_buffer_allocator->GetBuffer();
        }

        VertexBufferHandle AllocateVertexBuffer(ResourceFamily name, uint64_t size);
        IndexBufferHandle AllocateIndexBuffer(ResourceFamily name, uint64_t size);
        InstanceBufferHandle AllocateInstanceBuffer(ResourceFamily name, uint64_t size);

        BufferRegion& GetBufferRegion(VertexBufferHandle handle)
        {
            auto const family = std::to_underlying(handle.FamilyName());
            auto const index = handle.Index();

            assert(m_pools[family].vertex_buffer_allocator->regions.size() >= index);
            // TODO: family assert

            return m_pools[family].vertex_buffer_allocator->regions[index];
        }

        BufferRegion& GetBufferRegion(IndexBufferHandle handle)
        {
            auto const family = std::to_underlying(handle.FamilyName());
            auto const index = handle.Index();

            assert(m_pools[family].index_buffer_allocator->regions.size() >= index);
            // TODO: family assert

            return m_pools[family].index_buffer_allocator->regions[index];
        }

        BufferRegion& GetBufferRegion(InstanceBufferHandle handle)
        {
            auto const family = std::to_underlying(handle.FamilyName());
            auto const index = handle.Index();

            assert(m_pools[family].instance_buffer_allocator->regions.size() >= index);
            // TODO: family assert

            return m_pools[family].instance_buffer_allocator->regions[index];
        }

        void BeginUpload()
        {
            m_transfer_commands->Reset();
            m_transfer_commands->Begin();
        }

        void EndUpload()
        {
            m_transfer_commands->End();
        }

        // Call in render loop ---------------------- TODO: Typed TransferFuture....
        uint64_t Transfer()
        {
            auto const semaphore_value = m_transfer_buffer->TimelineValue();
            m_transfer_semaphore->Set(semaphore_value);
            m_transfer_queue->Submit(*m_transfer_commands, *m_transfer_semaphore);
            //m_transfer_semaphore->Wait();
            return semaphore_value;
        }

        // Call in render loop
        bool Complete(uint64_t timeline_number) const
        {
            auto const counter = m_transfer_semaphore->QueryCounter();
            m_transfer_buffer->Complete(counter); //---------------------------------- Is not threadsafe
            return counter >= timeline_number;
        }

        void Upload(VertexBufferHandle handle, std::function<void(BufferWriter&)> writer_callback)
        {
            Upload(GetBufferRegion(handle), writer_callback);
        }

        void Upload(IndexBufferHandle handle, std::function<void(BufferWriter&)> writer_callback)
        {
            Upload(GetBufferRegion(handle), writer_callback);
        }

        void Upload(InstanceBufferHandle handle, std::function<void(BufferWriter&)> writer_callback)
        {
            Upload(GetBufferRegion(handle), writer_callback);
        }

    private:
        void Upload(BufferRegion region, std::function<void(BufferWriter&)>& writer_callback)
        {
            assert(writer_callback != nullptr);

            auto staging_region = m_transfer_buffer->Allocate(region.Size()); // -------------- Reset complete allocations, see RingAllocator
            // TODO: Throw when vb_region is nullopt? --------------------------------------------------------
            auto staging_memory = m_transfer_buffer->Map<std::byte>(*staging_region);

            BufferWriter writer(staging_memory);
            writer_callback(writer);

            // -------- transfer command

            m_transfer_commands->TransferBuffer(*staging_region, region);

            //m_transfer_buffer->TimelineValue()
        }

        std::shared_ptr<Device> m_device;

        std::unique_ptr<TransferCommandQueue> m_transfer_queue;
        std::unique_ptr<TransferCommandBuffer> m_transfer_commands;

        // TODO: Mozna vice bufferu pro ruzne velikosti chunku allocatoru
        std::unique_ptr<BufferRingAllocator> m_transfer_buffer;

        std::unique_ptr<TimelineSemaphore> m_transfer_semaphore;

        // Map family index to a verte buffer regions.
        std::array<ResourcePool, 256> m_pools;
    };

} // Rc::Render