#include "resource.h"
#include "development.h"

namespace Rc::Render
{
    ResourceManager::ResourceManager(Device& device) :
        device{device}
    {}

    void ResourceManager::ReserveVertexBuffer(ResourceFamily family, uint64_t capacity)
    {
        auto const family_index = std::to_underlying(family);
        assert(family_index < pools.size());

        auto buffer = device.AllocateVertexBuffer(capacity);
        pools[family_index].vertex_buffer_allocator = std::make_unique<ResourceAllocator<VertexBufferHandle>>(std::move(buffer));
    }

    void ResourceManager::ReserveIndexBuffer(ResourceFamily family, uint64_t capacity)
    {
        auto const family_index = std::to_underlying(family);
        assert(family_index < pools.size());

        auto buffer = device.AllocateIndexBuffer(capacity);
        pools[family_index].index_buffer_allocator = std::make_unique<ResourceAllocator<IndexBufferHandle>>(std::move(buffer));
    }
    
    VertexBufferHandle ResourceManager::AllocateVertexBuffer(ResourceFamily name, uint64_t size)
    {
        return pools[std::to_underlying(name)].vertex_buffer_allocator->Allocate(name, size);
    }

    IndexBufferHandle ResourceManager::AllocateIndexBuffer(ResourceFamily name, uint64_t size)
    {
        return pools[std::to_underlying(name)].index_buffer_allocator->Allocate(name, size);
    }

    Texture2dHandle ResourceManager::AllocateTexture2d(
        ResourceFamily name,
        uint32_t width,
        uint32_t height,
        uint32_t mip_levels,
        PixelFormat format)
    {
        Rc::Dev::test_texture = device.AllocateTexture2d(width, height, mip_levels, format);
        return {}; // ----------------------------------------------------------------------
    }

    BufferRegion& ResourceManager::GetBufferRegion(VertexBufferHandle handle)
    {
        auto const family = std::to_underlying(handle.FamilyName());
        // TODO: family assert

        return pools[family].vertex_buffer_allocator->GetRegion(handle);
    }

    BufferRegion& ResourceManager::GetBufferRegion(IndexBufferHandle handle)
    {
        auto const family = std::to_underlying(handle.FamilyName());
        // TODO: family assert

        return pools[family].index_buffer_allocator->GetRegion(handle);
    }

    // ResourceUploader

    ResourceUploader::ResourceUploader(Device& device) : device{device}
    {
        transfer_queue = device.CreateTransferQueue();
        transfer_commands = transfer_queue->CreateCommandBuffer();

        transfer_buffer = std::make_unique<BufferRingAllocator>(
            device.AllocateStagingBuffer(BufferRingAllocator::default_chunk_size * 16),
            BufferRingAllocator::default_chunk_size
        );

        transfer_semaphore = device.CreateTimelineSemaphore();
    }

    uint64_t ResourceUploader::Upload(BufferRegion region, std::function<void(BufferWriter&)>& writer_callback)
    {
        assert(writer_callback != nullptr);

        auto staging_region = transfer_buffer->Allocate(region.Size()); // -------------- Reset complete allocations, see RingAllocator
        // TODO: Throw when vb_region is nullopt? Or Fallback --------------------------------------------------------
        auto staging_memory = transfer_buffer->Map<std::byte>(*staging_region);

        BufferWriter writer(staging_memory);
        writer_callback(writer);

        transfer_commands->MemoryBarrier(
            region, 
            BufferUsage::Undefined,
            BufferUsage::TransferWrite
        );
        
        transfer_commands->TransferBuffer(*staging_region, region);

        //-------------------------------- Release

        return transfer_buffer->TimelineValue();
    }

    uint64_t ResourceUploader::Upload(
        Texture2d& texture,
        RenderCommandQueue const& dst_queue,
        std::function<void(BufferWriter&)>& writer_callback)
    {
        assert(writer_callback != nullptr);

        auto staging_region = transfer_buffer->Allocate(texture.GetLinearDataSize());  // -------------- Reset complete allocations, see RingAllocator
        // TODO: Throw when vb_region is nullopt? Or Fallback --------------------------------------------------------
        auto staging_memory = transfer_buffer->Map<std::byte>(*staging_region);

        BufferWriter writer(staging_memory);
        writer_callback(writer);

        transfer_commands->Texture2dBarrier(
            texture,
            ImageUsage::Undefined,
            ImageUsage::TransferWrite
        );

        transfer_commands->TransferTexture(*staging_region, texture);

        transfer_commands->BarrierTexture2dRelease(
            texture,
            ImageUsage::TransferWrite,
            ImageUsage::SampledImage,
            transfer_queue->FamilyIndex(),
            dst_queue.FamilyIndex()
        );

        return transfer_buffer->TimelineValue();
    }

    void ResourceUploader::BeginUpload()
    {
        pending = false;
        transfer_commands->Reset();
        transfer_commands->Begin();
    }

    void ResourceUploader::EndUpload()
    {
        transfer_commands->End();
        pending = true;
    }
    
    void ResourceUploader::Transfer()
    {
        transfer_semaphore->Set(transfer_buffer->TimelineValue());
        transfer_queue->Submit(*transfer_commands, *transfer_semaphore);
        pending = false;
    }

    void ResourceUploader::QueryCounter()
    {
        counter = transfer_semaphore->QueryCounter();
    }

    bool ResourceUploader::Complete(uint64_t counter) const
    {
        return this->counter >= counter;
    }

} // Rc::Render