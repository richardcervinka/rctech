#include "resource.h"
#include "development.h"

namespace Rc::Render
{
    ResourceManager::ResourceManager(Device& device) :
        device{device}
    {}

    void ResourceManager::ReserveVertexBuffer(uint32_t family, uint64_t capacity)
    {
        assert(family < pools.size());

        auto buffer = device.AllocateVertexBuffer(capacity);
        pools[family].vertex_buffer_allocator = std::make_unique<ResourceAllocator<VertexBufferHandle>>(std::move(buffer));
    }

    void ResourceManager::ReserveIndexBuffer(uint32_t family, uint64_t capacity)
    {
        assert(family < pools.size());

        auto buffer = device.AllocateIndexBuffer(capacity);
        pools[family].index_buffer_allocator = std::make_unique<ResourceAllocator<IndexBufferHandle>>(std::move(buffer));
    }
    
    VertexBufferHandle ResourceManager::AllocateVertexBuffer(uint32_t family, uint64_t size)
    {
        return pools[family].vertex_buffer_allocator->Allocate(family, size);
    }

    IndexBufferHandle ResourceManager::AllocateIndexBuffer(uint32_t family, uint64_t size)
    {
        return pools[family].index_buffer_allocator->Allocate(family, size);
    }

    // Texture2DHandle ResourceManager::AllocateTexture2D(
    //     ResourceFamily name,
    //     uint32_t width,
    //     uint32_t height,
    //     Mips mips,
    //     PixelFormat format)
    // {
    //     Rc::Dev::test_texture = device.AllocateTexture2D(width, height, mips, format);
    //     return {}; // ----------------------------------------------------------------------
    // }

    BufferRegion& ResourceManager::GetBufferRegion(VertexBufferHandle handle)
    {

        // TODO: family assert

        return pools[handle.Family()].vertex_buffer_allocator->GetRegion(handle);
    }

    BufferRegion& ResourceManager::GetBufferRegion(IndexBufferHandle handle)
    {
        // TODO: family assert

        return pools[handle.Family()].index_buffer_allocator->GetRegion(handle);
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
        Texture2D& texture,
        RenderCommandQueue const& dst_queue,
        std::function<void(uint32_t mip, uint32_t w, uint32_t h, std::span<std::byte> dst)>& writer_callback)
    {
        assert(writer_callback != nullptr);
/*
        auto const layout = texture.Layout();

        // Texture size in bytes.
        uint32_t size = 0;
        for (auto mip : layout)
        {
            size += mip.size;
        }

        auto staging_region = transfer_buffer->Allocate(size);  // -------------- Reset complete allocations, see RingAllocator
        // TODO: Throw when vb_region is nullopt? Or Fallback --------------------------------------------------------
        auto staging_memory = transfer_buffer->Map<std::byte>(*staging_region);

        for (auto mip : layout)
        {
            writer_callback(
                mip.mip_level,
                mip.width,
                mip.height,
                staging_memory.subspan(mip.offset, mip.size));
        }

        //BufferWriter writer(staging_memory);
        // writer_callback(writer);

        // TextureRegion const src
        // {
        //     .buffer = *staging_region,
        //     .width = texture.Width(),
        //     .height = texture.Height(),
        //     .mip_level = 0,
        //     .array_level = 0
        // };

        transfer_commands->Texture2DBarrier(
            texture,
            ImageUsage::Undefined,
            ImageUsage::TransferWrite
        );

        transfer_commands->TransferTexture(*staging_region, texture);

        transfer_commands->BarrierTexture2DRelease(
            texture,
            ImageUsage::TransferWrite,
            ImageUsage::SampledImage,
            transfer_queue->FamilyIndex(),
            dst_queue.FamilyIndex()
        );

        return transfer_buffer->TimelineValue();
*/
        return 0;
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