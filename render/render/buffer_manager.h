#pragma once

#include "instance.h"
#include "platform/window.h"
#include "texture.h"
#include <array>
#include <functional>
#include "shader.h"
#include "descriptor_heap.h"
#include "core/camera.h"
#include "frame_renderer.h"
#include "buffer_linear_allocator.h"
#include "buffer_ring_allocator.h"

namespace Rc::Render
{
    enum class ResourceFamilyName : uint32_t {};

    class VertexBufferHandle
    {
    public:
        VertexBufferHandle() = default;

    private:
        friend class BufferManager;

        VertexBufferHandle(ResourceFamilyName family, uint64_t index) :
            m_family{family},
            m_index{index}
        {}

        ResourceFamilyName m_family {};
        uint64_t m_index {};
    };

    class IndexBufferHandle
    {
    public:
        IndexBufferHandle() = default;

    private:
        friend class BufferManager;

        IndexBufferHandle(ResourceFamilyName family, uint64_t index) :
            m_family{family},
            m_index{index}
        {}

        ResourceFamilyName m_family {};
        uint64_t m_index {};
    };

    class InstanceBufferHandle
    {
    public:
        InstanceBufferHandle() = default;

    private:
        friend class BufferManager;

        InstanceBufferHandle(ResourceFamilyName family, uint64_t index) :
            m_family{family},
            m_index{index}
        {}

        ResourceFamilyName m_family {};
        uint64_t m_index {};
    };

    struct ResourceFamily
    {
        std::unique_ptr<BufferLinearAllocator> vertex_buffer_allocator;
        std::unique_ptr<BufferLinearAllocator> instance_buffer_allocator;
        std::unique_ptr<BufferLinearAllocator> index_buffer_allocator;

        std::vector<BufferRegion> vertex_buffer_regions;
        std::vector<BufferRegion> instance_buffer_regions;
        std::vector<BufferRegion> index_buffer_regions;
    };

    class BufferManager
    {
    public:
        VertexBufferHandle AllocateVertexBuffer(ResourceFamilyName name, uint64_t size);
        IndexBufferHandle AllocateIndexBuffer(ResourceFamilyName name, uint64_t size);
        InstanceBufferHandle AllocateInstanceBuffer(ResourceFamilyName name, uint64_t size);

        // TODO: uint64_t capacity ...
        void CreateResourceFamily(
            ResourceFamilyName name,
            std::unique_ptr<Buffer> vertex_buffer,
            std::unique_ptr<Buffer> instance_buffer,
            std::unique_ptr<Buffer> index_buffer
        );

        BufferRegion& GetBufferRegion(VertexBufferHandle handle)
        {
            auto const family = std::to_underlying(handle.m_family);
            auto const index = handle.m_index;

            assert(m_resources[family].vertex_buffer_regions.size() >= index);
            // TODO: family assert

            return m_resources[family].vertex_buffer_regions[index];
        }

        BufferRegion& GetBufferRegion(IndexBufferHandle handle)
        {
            auto const family = std::to_underlying(handle.m_family);
            auto const index = handle.m_index;

            assert(m_resources[family].index_buffer_regions.size() >= index);
            // TODO: family assert

            return m_resources[family].index_buffer_regions[index];
        }

        BufferRegion& GetBufferRegion(InstanceBufferHandle handle)
        {
            auto const family = std::to_underlying(handle.m_family);
            auto const index = handle.m_index;

            assert(m_resources[family].instance_buffer_regions.size() >= index);
            // TODO: family assert

            return m_resources[family].instance_buffer_regions[index];
        }

        Buffer const& GetVertexBuffer(ResourceFamilyName family)
        {
            return m_resources[std::to_underlying(family)].vertex_buffer_allocator->GetBuffer();
        }

        Buffer const& GetInstanceBuffer(ResourceFamilyName family)
        {
            return m_resources[std::to_underlying(family)].instance_buffer_allocator->GetBuffer();
        }

        Buffer const& GetIndexBuffer(ResourceFamilyName family)
        {
            return m_resources[std::to_underlying(family)].index_buffer_allocator->GetBuffer();
        }

    private:
        // Map family index to a verte buffer regions.
        std::array<ResourceFamily, 256> m_resources; // --------------- Rename to Buffers
    };

} // Rc::Render