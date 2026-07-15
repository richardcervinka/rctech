#include "resource_manager.h"

namespace Rc::Render
{
    ResourceManager::ResourceManager(std::shared_ptr<Device> device) :
        m_device{std::move(device)}
    {
        m_transfer_queue = m_device->CreateTransferQueue();
        m_transfer_commands = m_transfer_queue->CreateCommandBuffer();

        StagingBufferInfo const transfer_buffer_info
        {
            .size = BufferRingAllocator::default_chunk_size * 16
        };
        
        m_transfer_buffer = std::make_unique<BufferRingAllocator>(
            m_device->AllocateBuffer(transfer_buffer_info),
            BufferRingAllocator::default_chunk_size
        );

        m_transfer_semaphore = m_device->CreateTimelineSemaphore();
    }

    void ResourceManager::ReserveVertexBuffer(ResourceFamily family, uint64_t capacity)
    {
        auto const family_index = std::to_underlying(family);
        assert(family_index < m_pools.size());

        auto buffer = m_device->AllocateBuffer(VertexBufferInfo{.size = capacity});
        m_pools[family_index].vertex_buffer_allocator = std::make_unique<ResourceAllocator<VertexBufferHandle>>(std::move(buffer));
    }

    void ResourceManager::ReserveInstanceBuffer(ResourceFamily family, uint64_t capacity)
    {
        auto const family_index = std::to_underlying(family);
        assert(family_index < m_pools.size());

        auto buffer = m_device->AllocateBuffer(VertexBufferInfo{.size = capacity});
        m_pools[family_index].instance_buffer_allocator = std::make_unique<ResourceAllocator<InstanceBufferHandle>>(std::move(buffer));
    }

    void ResourceManager::ReserveIndexBuffer(ResourceFamily family, uint64_t capacity)
    {
        auto const family_index = std::to_underlying(family);
        assert(family_index < m_pools.size());

        auto buffer = m_device->AllocateBuffer(IndexBufferInfo{.size = capacity});
        m_pools[family_index].index_buffer_allocator = std::make_unique<ResourceAllocator<IndexBufferHandle>>(std::move(buffer));
    }
    
    VertexBufferHandle ResourceManager::AllocateVertexBuffer(ResourceFamily name, uint64_t size)
    {
        return m_pools[std::to_underlying(name)].vertex_buffer_allocator->Allocate(name, size);
    }

    IndexBufferHandle ResourceManager::AllocateIndexBuffer(ResourceFamily name, uint64_t size)
    {
        return m_pools[std::to_underlying(name)].index_buffer_allocator->Allocate(name, size);
    }

    InstanceBufferHandle ResourceManager::AllocateInstanceBuffer(ResourceFamily name, uint64_t size)
    {
        return m_pools[std::to_underlying(name)].instance_buffer_allocator->Allocate(name, size);
    }

    BufferRegion& ResourceManager::GetBufferRegion(VertexBufferHandle handle)
    {
        auto const family = std::to_underlying(handle.FamilyName());
        auto const index = handle.Index();

        assert(m_pools[family].vertex_buffer_allocator->regions.size() >= index);
        // TODO: family assert

        return m_pools[family].vertex_buffer_allocator->regions[index];
    }

    BufferRegion& ResourceManager::GetBufferRegion(IndexBufferHandle handle)
    {
        auto const family = std::to_underlying(handle.FamilyName());
        auto const index = handle.Index();

        assert(m_pools[family].index_buffer_allocator->regions.size() >= index);
        // TODO: family assert

        return m_pools[family].index_buffer_allocator->regions[index];
    }

    BufferRegion& ResourceManager::GetBufferRegion(InstanceBufferHandle handle)
    {
        auto const family = std::to_underlying(handle.FamilyName());
        auto const index = handle.Index();

        assert(m_pools[family].instance_buffer_allocator->regions.size() >= index);
        // TODO: family assert

        return m_pools[family].instance_buffer_allocator->regions[index];
    }

    void ResourceManager::Upload(BufferRegion region, std::function<void(BufferWriter&)>& writer_callback)
    {
        assert(writer_callback != nullptr);

        auto staging_region = m_transfer_buffer->Allocate(region.Size()); // -------------- Reset complete allocations, see RingAllocator
        // TODO: Throw when vb_region is nullopt? Or Fallback --------------------------------------------------------
        auto staging_memory = m_transfer_buffer->Map<std::byte>(*staging_region);

        BufferWriter writer(staging_memory);
        writer_callback(writer);

        // -------- transfer command

        m_transfer_commands->TransferBuffer(*staging_region, region);

        //m_transfer_buffer->TimelineValue()
    }

    void ResourceManager::BeginUpload()
    {
        m_transfer_commands->Reset();
        m_transfer_commands->Begin();
    }

    void ResourceManager::EndUpload()
    {
        m_transfer_commands->End();
        m_pending = true;
    }
    
    void ResourceManager::Transfer()
    {
        m_transfer_semaphore->Set(m_transfer_buffer->TimelineValue());
        m_transfer_queue->Submit(*m_transfer_commands, *m_transfer_semaphore);
        m_pending = false;
    }

    void ResourceManager::QueryCounter()
    {
        m_counter = m_transfer_semaphore->QueryCounter();
    }

    bool ResourceManager::Complete(uint64_t counter) const
    {
        return m_counter >= counter;
    }

} // Rc::Render