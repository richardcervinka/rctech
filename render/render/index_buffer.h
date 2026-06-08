#pragma once

#include "buffer.h"
#include <cstddef>
#include <cstdint>

namespace Rc::Render
{
    enum class IndexType
    {
        Uint16,
        Uint32
    };

    struct IndexBufferView
    {
        uint64_t offset;
        uint64_t size;
        IndexType type;
    };

    struct IndexBufferParams
    {
        uint64_t size;
        IndexType type;
    };


} // Rc::Render
