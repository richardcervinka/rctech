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

} // Rc::Render