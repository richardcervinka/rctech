#include "texture.h"
#include "error.h"

namespace Rc::Render
{
    std::unique_ptr<RenderTargetView> Texture2D::CreateView() const
    {
        return std::make_unique<RenderTargetView>(vk_device, vk_image, vk_format, width, height);
    }

} // Rc::Render