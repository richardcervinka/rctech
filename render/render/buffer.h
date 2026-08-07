#pragma once

#include "vulkan/device.h"
#include <span>
#include <cstddef>
#include <cassert>
#include "base/float.h"
#include "base/vector.h"
#include "base/matrix.h"

namespace Rc::Render
{
    // GPU buffer region descriptor
    class BufferRegion
    {
    public:
        BufferRegion() = default;
        ~BufferRegion() = default;
        BufferRegion(BufferRegion const&) = default;
        BufferRegion& operator=(BufferRegion const&) = default;
        BufferRegion(BufferRegion&&) = default;
        BufferRegion& operator=(BufferRegion&&) = default;

        uint64_t Offset() const
        {
            return offset;
        }
        
        uint64_t Size() const
        {
            return size;
        }
        
        bool Empty() const
        {
            return size == 0;
        }

        VkBuffer Handle() const
        {
            return vk_buffer;
        }

    private:
        friend class Buffer;
        friend class RenderCommandBuffer;
        friend class TransferCommandBuffer;

        BufferRegion(VkBuffer buffer, uint64_t offset, uint64_t size) :
            vk_buffer{buffer},
            offset{offset},
            size{size}
        {}

        VkBuffer vk_buffer {VK_NULL_HANDLE};
        uint64_t offset {0};
        uint64_t size {0};
        //uint64_t generation {0};

        VkAccessFlags2 access_flags {VK_ACCESS_NONE};
        VkPipelineStageFlags stage_flags {VK_PIPELINE_STAGE_NONE};
    };

    //
    // GPU buffer
    //
    class Buffer
    {
    public:
        Buffer(
            VulkanDevice const& vk_device,
            uint64_t size,
            VmaAllocator vma_allocator,
            VkBufferUsageFlags2 usage_flags,
            VmaAllocationCreateFlags vma_flags
        );

        ~Buffer();

        Buffer(Buffer const&) = delete;
        Buffer& operator=(Buffer const&) = delete;
        Buffer(Buffer&& other) = delete;
        Buffer& operator=(Buffer&& other) = delete;

        VkBuffer Handle() const
        {
            return vk_buffer;
        }

        VkDeviceAddress Address() const;

        uint64_t Size() const
        {
            return vma_allocation_info.size;
        }

        BufferRegion GetRegion() const;
        BufferRegion GetRegion(uint64_t offset, uint64_t size) const;

        // Available only for stagging buffer.
        std::span<std::byte> Buffer::Map(uint64_t offset, uint64_t size);
        std::span<std::byte const> Buffer::Map(uint64_t offset, uint64_t size) const;

        std::span<std::byte> Buffer::Map()
        {
            return Map(0, Size());
        }

        std::span<std::byte> Buffer::Map(BufferRegion const& region)
        {
            return Map(region.Offset(), region.Size());
        }

        std::span<std::byte const> Buffer::Map(BufferRegion const& region) const
        {
            return Map(region.Offset(), region.Size());
        }

        // template<typename T>
        // std::span<T> Map(BufferRegion const& region)
        // {
        //     auto raw = m_buffer.Map(region);

        //     return {
        //         reinterpret_cast<T*>(raw.data()),
        //         region.Size() / sizeof(T)
        //     };
        // }

    private:


        VulkanDevice const& vk_device;
        VkBuffer vk_buffer {VK_NULL_HANDLE};
        VmaAllocator vma_allocator {VK_NULL_HANDLE};
        VmaAllocation vma_allocation {nullptr};
        VmaAllocationInfo vma_allocation_info {};
    };

    class BufferWriter
    {
    public:
        explicit BufferWriter(std::span<std::byte> dst) :
            ptr{dst.data()}
            //m_end{dst.end()}
        {}

        void Write(float value)
        {
            *reinterpret_cast<float*>(ptr) = value;
            ptr += sizeof(float);
        }

        void Write(uint32_t value)
        {
            *reinterpret_cast<uint32_t*>(ptr) = value;
            ptr += sizeof(uint32_t);
        }

        void Write(int32_t value)
        {
            *reinterpret_cast<uint32_t*>(ptr) = value;
            ptr += sizeof(uint32_t);
        }

        void Write(std::span<std::byte const> value)
        {
            std::memcpy(ptr, value.data(), value.size());
            ptr += value.size();
        }

        BufferWriter& operator<<(float value)
        {
            Write(value);
            return *this;
        }

        BufferWriter& operator<<(Vector4<float> value)
        {
            //value.
            Write(value.x);
            Write(value.y);
            Write(value.z);
            Write(value.w);
            return *this;
        }

        BufferWriter& operator<<(Float4 value)
        {
            value.Store(ptr);
            ptr += 4 * sizeof(float);
            return *this;
        }

        BufferWriter& operator<<(Float3 value)
        {
            value.Store(ptr);
            ptr += 3 * sizeof(float);
            return *this;
        }

        BufferWriter& operator<<(Float2 value)
        {
            value.Store(ptr);
            ptr += 2 * sizeof(float);
            return *this;
        }

        BufferWriter& operator<<(std::span<uint16_t const> value)
        {
            Write(std::as_bytes(value));
            return *this;
        }

        BufferWriter& operator<<(Matrix4<float> const& value)
        {
            std::span const raw{ptr, sizeof(float) * 16};
            value.Store(raw);
            ptr += raw.size();
            return *this;
        }

    private:
        std::byte* ptr {nullptr};
        //std::byte const* m_end {nullptr};
        //std::span<std::byte> m_dst;
        
    };

} // Rc::Render