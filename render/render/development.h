#pragma once

#include "texture.h"

namespace Rc
{
    class Dev
    {
    public:
        static inline std::unique_ptr<Render::Texture2d> test_texture;
    };

    // Replace Renderer::test_model
    // std::unique_ptr<TestModel> test_model;

} // Rc