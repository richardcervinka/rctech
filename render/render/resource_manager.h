#pragma once

#include <array>
#include <memory>
#include <atomic>
#include <mutex>
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
            family{family},
            index{index},
            generation{generation}
        {}

        ResourceFamily FamilyName() const
        {
            return family;
        }

        uint64_t Index() const
        {
            return index;
        }

        uint32_t Generation() const
        {
            return generation;
        }
        
    private:
        ResourceFamily family {};
        uint64_t index {};
        uint32_t generation {};
    };

    class IndexBufferHandle
    {
    public:
        IndexBufferHandle() = default;

        IndexBufferHandle(ResourceFamily family, uint64_t index, uint32_t generation) :
            family{family},
            index{index},
            generation{generation}
        {}

        ResourceFamily FamilyName() const
        {
            return family;
        }

        uint64_t Index() const
        {
            return index;
        }

        uint32_t Generation() const
        {
            return generation;
        }

    private:
        ResourceFamily family {};
        uint64_t index {};
        uint32_t generation {};
    };

    class InstanceBufferHandle
    {
    public:
        InstanceBufferHandle() = default;

        InstanceBufferHandle(ResourceFamily family, uint64_t index, uint32_t generation) : // ----- Private?
            family{family},
            index{index},
            generation{generation}
        {}

        ResourceFamily FamilyName() const
        {
            return family;
        }

        uint64_t Index() const
        {
            return index;
        }

        uint32_t Generation() const
        {
            return generation;
        }

    private:
        ResourceFamily family {};
        uint64_t index {};
        uint32_t generation {};
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
            return pools[std::to_underlying(family)].vertex_buffer_allocator->GetBuffer();
        }

        Buffer const& GetInstanceBuffer(ResourceFamily family)
        {
            return pools[std::to_underlying(family)].instance_buffer_allocator->GetBuffer();
        }

        Buffer const& GetIndexBuffer(ResourceFamily family)
        {
            return pools[std::to_underlying(family)].index_buffer_allocator->GetBuffer();
        }

        VertexBufferHandle AllocateVertexBuffer(ResourceFamily name, uint64_t size);
        IndexBufferHandle AllocateIndexBuffer(ResourceFamily name, uint64_t size);
        InstanceBufferHandle AllocateInstanceBuffer(ResourceFamily name, uint64_t size);

        BufferRegion& GetBufferRegion(VertexBufferHandle handle);
        BufferRegion& GetBufferRegion(IndexBufferHandle handle);
        BufferRegion& GetBufferRegion(InstanceBufferHandle handle);

        void BeginUpload();

        void EndUpload();

        // Call in render loop ---------------------- TODO: Typed TransferFuture....
        void Transfer();

        // Call in render loop
        void QueryCounter();

        // Call in render loop
        bool Complete(uint64_t counter) const;

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

        // Lockable interface
        bool try_lock()
        {
            return mutex.try_lock();
        }

        // Basic-Lockable interface
        void lock()
        {
            mutex.lock();
        }

        // Basic-Lockable interface
        void unlock()
        {
            mutex.unlock();
        }

        bool PendingTransfer() const
        {
            return pending;
        }

    private:
        void Upload(BufferRegion region, std::function<void(BufferWriter&)>& writer_callback);

        std::shared_ptr<Device> device;

        std::unique_ptr<TransferCommandQueue> transfer_queue;
        std::unique_ptr<TransferCommandBuffer> transfer_commands;

        // TODO: Mozna vice bufferu pro ruzne velikosti chunku allocatoru
        std::unique_ptr<BufferRingAllocator> transfer_buffer;

        std::unique_ptr<TimelineSemaphore> transfer_semaphore;

        // Map family index to a verte buffer regions.
        std::array<ResourcePool, 256> pools;

        std::mutex mutex;

        uint64_t counter {0};

        std::atomic<bool> pending {false};
    };

} // Rc::Render