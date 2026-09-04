#pragma once

#include <span>
#include <cstddef>
#include <cassert>
#include "base/float.h"
#include "base/vector.h"
#include "base/matrix.h"

namespace Rc::Render
{
    class BufferWriter
    {
    public:
        BufferWriter() = default;
        
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

        void Write(std::span<uint8_t const> value)
        {
            std::memcpy(ptr, value.data(), value.size());
            ptr += value.size();
        }

        BufferWriter& operator<<(uint32_t value)
        {
            Write(value);
            return *this;
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

        BufferWriter& operator<<(std::span<uint8_t const> value)
        {
            Write(value);
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