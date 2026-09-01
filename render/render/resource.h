#pragma once

#include <array>
#include <memory>
#include <atomic>
#include <mutex>
#include "buffer_linear_allocator.h"
#include "buffer_ring_allocator.h"
#include "buffer_writer.h"
#include "render/device.h"

namespace Rc::Render
{
    enum class ResourceFamily : uint32_t {};

    enum class ResourceType
    {
        VertexBuffer,
        IndexBuffer,
        Texture
    };

    template<ResourceType>
    class ResourceHandle
    {
    public:
        using Type = ResourceType;

        ResourceHandle() = default;

        ResourceHandle(ResourceFamily family, uint64_t index, uint32_t generation) :
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

    using VertexBufferHandle = ResourceHandle<ResourceType::VertexBuffer>;
    using IndexBufferHandle = ResourceHandle<ResourceType::IndexBuffer>;
    using Texture2dHandle = ResourceHandle<ResourceType::Texture>;

    //
    // Linear subresource handle allocator.
    // The Handle must be of type ResourceHandle<T>.
    //
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

        BufferRegion& GetRegion(Handle handle)
        {
            assert(regions.size() >= handle.Index());

            return regions[handle.Index()];
        }

    private:
        BufferLinearAllocator allocator;

        std::atomic<std::size_t> count {0};

        // Map handle index to a buffer region.
        std::array<BufferRegion, UINT16_MAX> regions;

        // TODO: Generation
    };

    //
    // Resource pool associated with a resource family.
    //
    struct ResourcePool
    {
        std::unique_ptr<ResourceAllocator<VertexBufferHandle>> vertex_buffer_allocator;
        std::unique_ptr<ResourceAllocator<IndexBufferHandle>> index_buffer_allocator;
        //std::vector<TextureHandle> textures;
    };

    //
    // Upload GPU data using staging buffer.
    //
    class ResourceManager
    {
    public:
        explicit ResourceManager(Device& device);

        void ReserveVertexBuffer(ResourceFamily family, uint64_t capacity); // usage parameter?
        void ReserveInstanceBuffer(ResourceFamily family, uint64_t capacity); // usage parameter?
        void ReserveIndexBuffer(ResourceFamily family, uint64_t capacity); // usage parameter?

        VertexBufferHandle AllocateVertexBuffer(ResourceFamily name, uint64_t size);
        IndexBufferHandle AllocateIndexBuffer(ResourceFamily name, uint64_t size);
        Texture2dHandle AllocateTexture2d(ResourceFamily name, uint32_t width, uint32_t height, PixelFormat format);

        Buffer const& GetVertexBuffer(ResourceFamily family)
        {
            return pools[std::to_underlying(family)].vertex_buffer_allocator->GetBuffer();
        }

        Buffer const& GetIndexBuffer(ResourceFamily family)
        {
            return pools[std::to_underlying(family)].index_buffer_allocator->GetBuffer();
        }

        BufferRegion& GetBufferRegion(VertexBufferHandle handle);
        BufferRegion& GetBufferRegion(IndexBufferHandle handle);

    private:
        Device& device; // ---------------------------------------- const ref ?

        // Map family index to a verte buffer regions.
        std::array<ResourcePool, 256> pools;
    };

    class ResourceUploader
    {
    public:
        ResourceUploader(Device& device);

        void BeginUpload();

        void EndUpload();

        // Call in render loop
        void Transfer();

        // Call in render loop
        void QueryCounter();

        // Call in render loop
        bool Complete(uint64_t counter) const;

        // TODO: Renam to UploadBuffer, UploadTexture2d
        uint64_t Upload(
            BufferRegion region,
            std::function<void(BufferWriter&)>& writer_callback
        );

        uint64_t Upload(
            Texture2d& texture,
            RenderCommandQueue const& dst_queue,
            std::function<void(BufferWriter&)>& writer_callback
        );

        bool PendingTransfer() const
        {
            return pending;
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

    private:
        Device& device;

        std::unique_ptr<TransferCommandQueue> transfer_queue;
        std::unique_ptr<TransferCommandBuffer> transfer_commands;

        // TODO: Mozna vice bufferu pro ruzne velikosti chunku allocatoru
        std::unique_ptr<BufferRingAllocator> transfer_buffer;

        std::unique_ptr<TimelineSemaphore> transfer_semaphore;

        std::mutex mutex;

        uint64_t counter {0};

        std::atomic<bool> pending {false};
    };

} // Rc::Render