#pragma once

#include <array>
#include <memory>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <numeric>
#include <filesystem>
#include "buffer_linear_allocator.h"
#include "buffer_ring_allocator.h"
#include "buffer_writer.h"
#include "render/device.h"
#include <deque>
#include <expected>

namespace Rc::Render
{
    enum class ResourceType
    {
        VertexBuffer,
        IndexBuffer,
        Texture
    };

    using ResourceFamily = uint32_t;

    template<ResourceType>
    class ResourceHandle
    {
    public:
        ResourceHandle() = default;

        ResourceHandle(uint32_t family, uint32_t slot, uint32_t index) :
            family{family},
            slot{slot},
            index{index}
        {}

        ResourceFamily Family() const
        {
            return family;
        }
        
        // Resource unique identifier.
        uint32_t Slot() const
        {
            return slot;
        }

        // Shader resource descriptor index.
        uint32_t Index() const
        {
            return index;
        }
        
    private:
        uint32_t family {0};
        uint32_t slot {0};
        uint32_t index {};
    };

    using VertexBufferHandle = ResourceHandle<ResourceType::VertexBuffer>;
    using IndexBufferHandle = ResourceHandle<ResourceType::IndexBuffer>;
    using Texture2DHandle = ResourceHandle<ResourceType::Texture>;

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

        Handle Allocate(uint32_t family, uint64_t size)
        {
            auto const id = count.fetch_add(1, std::memory_order_relaxed);
            regions[id] = allocator.Allocate(size);
            return {family, 0, static_cast<uint32_t>(id)}; // ---------------------------------
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

    class ResourceManager
    {
    public:
        explicit ResourceManager(Device& device);

        void ReserveVertexBuffer(uint32_t family, uint64_t capacity); // usage parameter?
        void ReserveInstanceBuffer(uint32_t family, uint64_t capacity); // usage parameter?
        void ReserveIndexBuffer(uint32_t family, uint64_t capacity); // usage parameter?

        VertexBufferHandle AllocateVertexBuffer(uint32_t family, uint64_t size);
        IndexBufferHandle AllocateIndexBuffer(uint32_t family, uint64_t size);

        // Texture2DHandle AllocateTexture2D(
        //     ResourceFamily name,
        //     uint32_t width,
        //     uint32_t height,
        //     Mips mips,
        //     PixelFormat format
        // );

        Buffer const& GetVertexBuffer(uint32_t family)
        {
            return pools[family].vertex_buffer_allocator->GetBuffer();
        }

        Buffer const& GetIndexBuffer(uint32_t family)
        {
            return pools[family].index_buffer_allocator->GetBuffer();
        }

        BufferRegion& GetBufferRegion(VertexBufferHandle handle);
        BufferRegion& GetBufferRegion(IndexBufferHandle handle);

    private:
        Device& device; // ---------------------------------------- const ref ?

        // Map family index to a verte buffer regions.
        std::array<ResourcePool, 256> pools;
    };

    //
    // Upload GPU data using staging buffer.
    //
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

        // TODO: Renam to UploadBuffer, UploadTexture2D
        uint64_t Upload(
            BufferRegion region,
            std::function<void(BufferWriter&)>& writer_callback
        );

        uint64_t Upload(
            Texture2D& texture,
            RenderCommandQueue const& dst_queue,
            std::function<void(uint32_t mip, uint32_t w, uint32_t h, std::span<std::byte> dst)>& writer_callback
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

    //
    // Named reserved texture slots
    //
    // enum class TexturSlot : uint32_t
    // {
    //     Default = 0
    // };

    using TextureSlot = uint32_t;

    //
    // Map public texture slot to the Vulkan objects
    //
    class TextureManager
    {
    public:
        static inline const uint32_t slot_count = UINT16_MAX;

        void InsertTexture2D(std::unique_ptr<Texture2D> texture, TextureSlot slot, uint32_t index)
        {
            assert(slot < slot_count);

            slots[slot].texture = std::move(texture);
            slots[slot].index = index;
        }

        void ReleaseTexture2D(TextureSlot slot)
        {
            assert(slot < slot_count);

            slots[slot] = {};
        }

        Texture2D const& GetTexture2D(TextureSlot slot) const
        {
            return *slots[slot].texture;
        }

        uint32_t GetIndex(TextureSlot slot) const
        {
            return slots[slot].index;
        }

    private:
        struct Slot
        {
            std::unique_ptr<Texture2D> texture;

            // Descriptor heap index
            uint32_t index {0};

            // Optional
            // std::string name;
        };

        std::array<Slot, slot_count> slots;
    };







    

    class TextureLoader
    {
    public:
        enum class Error
        {
            OutOfMemory
        };

        explicit TextureLoader(std::unique_ptr<Buffer> buffer);

        std::expected<BufferRegion, Error> Load(std::filesystem::path path);

        TextureInfo GetTextureInfo(BufferRegion const& region) const;

        void Reset()
        {
            allocator.Reset();
        }

    private:
        BufferLinearAllocator allocator;
    };







    class TextureUploader
    {
    public:
        TextureUploader(
            Device const& device
            // std::unique_ptr<TransferCommandQueue> queue,
            // std::unique_ptr<TransferCommandBuffer> commands
        );

        // Create a load request.
        // Returns unuque identifier.
        void Transfer(BufferRegion const& src, std::span<TextureLayout const> layout, Texture2D& dst);

        void Submit();

        // void Wait();

        //-------------------

        bool Complete() const
        {
            return semaphore->QueryCounter() >= submit_counter;
        }

    private:
        std::unique_ptr<TextureLoader> loader;
        //std::unique_ptr<TextureLoader> submitted_loader;

        // void Loop();
        
        // std::jthread thread;
        // std::atomic<uint32_t> wait_counter {0};
        // std::atomic<bool> terminate {false};
        
        Device const& device;
        
        BufferRingAllocator buffer;

        std::unique_ptr<TransferCommandQueue> queue; //----------------------- take pro back buffer !!!!!!

        //std::unique_ptr<TransferCommandBuffer> submitted_commands;

        std::unique_ptr<TransferCommandBuffer> commands;

        std::unique_ptr<TimelineSemaphore> semaphore;

        // Snapshot of the buffer ring allocator.
        uint64_t submit_counter {0};

        uint64_t uid_counter {0};
    };

} // Rc::Render