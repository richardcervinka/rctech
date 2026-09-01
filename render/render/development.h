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

        static inline std::unique_ptr<Render::Texture2d> test_texture;

        static inline Rc::Render::VertexBufferHandle vb_handle;
        static inline Rc::Render::IndexBufferHandle ib_handle; 

        static inline std::vector<Rc::Gfx::Transformations> instances;

        static inline uint64_t resource_timeline {0};
    };

} // Rc