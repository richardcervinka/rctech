#pragma once

#include "core/camera.h"
#include "base/matrix.h"
#include "buffer.h"
#include <cassert>

namespace Rc::Render
{
    struct alignas(16) RenderPassConstants
    {
        Matrix4<double> camera_projection_matrix;

        static constexpr uint64_t size {sizeof(float) * 16};

        void Write(Buffer& dst, BufferRegion region) const
        {
            assert(region.Size() >= size);
            
            auto data = dst.Map(region);

            camera_projection_matrix.StoreAs<float>(data);
        }
    };

} // Rc::Render