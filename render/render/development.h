#pragma once

#include "texture.h"
#include "resource.h"
#include "core/transformations.h"

namespace Rc
{
    class Dev
    {
    public:
        static inline bool initialized {false};

        //static inline std::unique_ptr<Render::Texture2D> test_texture;

        static inline Render::VertexBufferHandle vb_handle;
        static inline Render::IndexBufferHandle ib_handle; 

        static inline std::vector<Gfx::Transformations> instances;

        static inline uint64_t resource_timeline {0};
    };

} // Rc