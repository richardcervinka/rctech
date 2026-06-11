#pragma once

#include "base/matrix.h"

namespace Rc::Render {

    // Create reverse‑Z perspective projection matrix
    Matrix4<float> CreatePerspectiveProjectionMatrix(
        int width,
        int height,
        double vertical_fov,
        double z_near,
        double z_far
    );

} // Rc::Render