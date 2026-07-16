#include "resource_manager.h"

namespace Rc::Render
{
    ResourceManager::ResourceManager(std::shared_ptr<Device> device) :
        device{std::move(device)}
    {
        transfer_queue = this->device->CreateTransferQueue();
        transfer_commands = transfer_queue->CreateCommandBuffer();

        StagingBufferInfo const transfer_buffer_info
        {
            .size = BufferRingAllocator::default_chunk_size * 16
        };
        
        transfer_buffer = std::make_unique<BufferRingAllocator>(
            this->device->AllocateBuffer(transfer_buffer_info),
            BufferRingAllocator::default_chunk_size
        );

        transfer_semaphore = this->device->CreateTimelineSemaphore();
    }

    void ResourceManager::ReserveVertexBuffer(ResourceFamily family, uint64_t capacity)
    {
        auto const family_index = std::to_underlying(family);
        assert(family_index < pools.size());

        auto buffer = device->AllocateBuffer(VertexBufferInfo{.size = capacity});
        pools[family_index].vertex_buffer_allocator = std::make_unique<ResourceAllocator<VertexBufferHandle>>(std::move(buffer));
    }

    void ResourceManager::ReserveInstanceBuffer(ResourceFamily family, uint64_t capacity)
    {
        auto const family_index = std::to_underlying(family);
        assert(family_index < pools.size());

        auto buffer = device->AllocateBuffer(VertexBufferInfo{.size = capacity});
        pools[family_index].instance_buffer_allocator = std::make_unique<ResourceAllocator<InstanceBufferHandle>>(std::move(buffer));
    }

    void ResourceManager::ReserveIndexBuffer(ResourceFamily family, uint64_t capacity)
    {
        auto const family_index = std::to_underlying(family);
        assert(family_index < pools.size());

        auto buffer = device->AllocateBuffer(IndexBufferInfo{.size = capacity});
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

    InstanceBufferHandle ResourceManager::AllocateInstanceBuffer(ResourceFamily name, uint64_t size)
    {
        return pools[std::to_underlying(name)].instance_buffer_allocator->Allocate(name, size);
    }

    BufferRegion& ResourceManager::GetBufferRegion(VertexBufferHandle handle)
    {
        auto const family = std::to_underlying(handle.FamilyName());
        auto const index = handle.Index();

        assert(pools[family].vertex_buffer_allocator->regions.size() >= index);
        // TODO: family assert

        return pools[family].vertex_buffer_allocator->regions[index];
    }

    BufferRegion& ResourceManager::GetBufferRegion(IndexBufferHandle handle)
    {
        auto const family = std::to_underlying(handle.FamilyName());
        auto const index = handle.Index();

        assert(pools[family].index_buffer_allocator->regions.size() >= index);
        // TODO: family assert

        return pools[family].index_buffer_allocator->regions[index];
    }

    BufferRegion& ResourceManager::GetBufferRegion(InstanceBufferHandle handle)
    {
        auto const family = std::to_underlying(handle.FamilyName());
        auto const index = handle.Index();

        assert(pools[family].instance_buffer_allocator->regions.size() >= index);
        // TODO: family assert

        return pools[family].instance_buffer_allocator->regions[index];
    }

    void ResourceManager::Upload(BufferRegion region, std::function<void(BufferWriter&)>& writer_callback)
    {
        assert(writer_callback != nullptr);

        auto staging_region = transfer_buffer->Allocate(region.Size()); // -------------- Reset complete allocations, see RingAllocator
        // TODO: Throw when vb_region is nullopt? Or Fallback --------------------------------------------------------
        auto staging_memory = transfer_buffer->Map<std::byte>(*staging_region);

        BufferWriter writer(staging_memory);
        writer_callback(writer);

        // -------- transfer command

        transfer_commands->TransferBuffer(*staging_region, region);

        //m_transfer_buffer->TimelineValue()
    }

    void ResourceManager::BeginUpload()
    {
        transfer_commands->Reset();
        transfer_commands->Begin();
    }

    void ResourceManager::EndUpload()
    {
        transfer_commands->End();
        pending = true;
    }
    
    void ResourceManager::Transfer()
    {
        transfer_semaphore->Set(transfer_buffer->TimelineValue());
        transfer_queue->Submit(*transfer_commands, *transfer_semaphore);
        pending = false;
    }

    void ResourceManager::QueryCounter()
    {
        counter = transfer_semaphore->QueryCounter();
    }

    bool ResourceManager::Complete(uint64_t counter) const
    {
        return this->counter >= counter;
    }

} // Rc::Render