#include "buffer_manager.h"

namespace Rc::Render
{
    void BufferManager::CreateResourceFamily(
        ResourceFamilyName name,
        std::unique_ptr<Buffer> vertex_buffer,
        std::unique_ptr<Buffer> instance_buffer,
        std::unique_ptr<Buffer> index_buffer)
    {
        assert(std::to_underlying(name) < m_resources.size());

        auto const index = std::to_underlying(name);

        {
            m_resources[index].vertex_buffer_allocator = std::make_unique<BufferLinearAllocator>(std::move(vertex_buffer));
            m_resources[index].vertex_buffer_regions.reserve(UINT16_MAX); // --------------- DO NOT USE UINT16_MAX
        }
        {
            m_resources[index].instance_buffer_allocator = std::make_unique<BufferLinearAllocator>(std::move(instance_buffer));
            m_resources[index].instance_buffer_regions.reserve(UINT16_MAX); // --------------- DO NOT USE UINT16_MAX
        }
        {
            m_resources[index].index_buffer_allocator = std::make_unique<BufferLinearAllocator>(std::move(index_buffer));
            m_resources[index].index_buffer_regions.reserve(UINT16_MAX); // --------------- DO NOT USE UINT16_MAX
        }
    }
    
    VertexBufferHandle BufferManager::AllocateVertexBuffer(ResourceFamilyName name, uint64_t size)
    {
        auto& resources = m_resources[std::to_underlying(name)];

        assert(resources.vertex_buffer_allocator != nullptr);

        auto const region = resources.vertex_buffer_allocator->Allocate(size);
        uint64_t const uid = resources.vertex_buffer_regions.size();
        resources.vertex_buffer_regions.push_back(region);

        return {name, uid};
    }

    IndexBufferHandle BufferManager::AllocateIndexBuffer(ResourceFamilyName name, uint64_t size)
    {
        auto& resources = m_resources[std::to_underlying(name)];

        assert(resources.index_buffer_allocator != nullptr);

        auto const region = resources.index_buffer_allocator->Allocate(size);
        uint64_t const uid = resources.index_buffer_regions.size();
        resources.index_buffer_regions.push_back(region);

        return {name, uid};
    }

    InstanceBufferHandle BufferManager::AllocateInstanceBuffer(ResourceFamilyName name, uint64_t size)
    {
        auto& resources = m_resources[std::to_underlying(name)];

        assert(resources.instance_buffer_allocator != nullptr);

        auto const region = resources.instance_buffer_allocator->Allocate(size);
        uint64_t const uid = resources.instance_buffer_regions.size();
        resources.instance_buffer_regions.push_back(region);

        return {name, uid};
    }

} // Rc::Render