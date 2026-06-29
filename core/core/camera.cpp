#include "camera.h"
#include <cassert>

namespace Rc::Gfx
{
    // Reverse‑Z perspective projection matrix (column-major, column vectors).
    //
    // Parameters:
    // - fovY   – vertical field of view in radians
    // - aspect – aspect ratio (width / height)
    // - zNear  – distance to the near clipping plane (> 0)
    // - zFar   – distance to the far clipping plane (> zNear)
    //
    // Reverse‑Z:
    // - Depth range is reversed: near plane maps to depth = 1.0, far plane to depth = 0.0.
    // - Depth test must use GREATER instead of LESS.
    // - Depth buffer should be floating-point (e.g., D32F).
    // - Greatly improves depth precision, especially at large distances.
    //
    // Notes:
    // - Designed for column-major storage and column-vector multiplication: v' = P * v.
    // - Produces NDC depth in range [1 (near), 0 (far)].
    // - The -1 in the last row (third column) performs the perspective divide.
    // - Reverse‑Z significantly reduces Z‑fighting compared to the classic projection matrix.
    //
    static Matrix4<double> CreatePerspectiveProjectionMatrix(
        int width,
        int height,
        double vertical_fov,
        double z_near,
        double z_far)
    {
        double const aspect = static_cast<double>(width) / static_cast<double>(height);
        double const tanfov = std::tan(vertical_fov / 2.0);

        return
        {
            1.0 / (tanfov * aspect),
            0,
            0,
            0,
            0,
            1.0 / tanfov,
            0,
            0,
            0,
            0,
            z_near / (z_far - z_near),
            (z_far * z_near) / (z_far - z_near),
            0,
            0,
            -1,
            0
        };
    }
    
    Matrix4<double> PerspectiveCamera::GetProjectionMatrix(int viewport_width, int viewport_height) const
    {
        assert(fov > min_fov);
        assert(fov < max_fov);

        auto const projection_matrix = CreatePerspectiveProjectionMatrix(
            viewport_width,
            viewport_height,
            fov,
            z_near,
            z_far
        );

        auto transformations_norm = transformations;
        transformations_norm.scale = 1.0;

        auto result = transformations_norm.GetTransformations();
        result.AppendTransformations(projection_matrix);

        return result;
    }

} // Rc::Gfx


/*
    Perspective projection matrix (column-major, column vectors).

    Parameters:
        fovY   – vertical field of view in radians
        aspect – aspect ratio (width / height)
        zNear  – distance to the near clipping plane (> 0)
        zFar   – distance to the far clipping plane (> zNear)

    The resulting matrix transforms a 3D point v = (x, y, z, 1)
    into clip space using:  v' = P * v

    Mathematical form:

        |  1 / (tan(fovY/2) * aspect)      0                     0                     0 |
        |  0                               1 / tan(fovY/2)       0                     0 |
        |  0                               0        (zFar + zNear) / (zNear - zFar)   (2 * zFar * zNear) / (zNear - zFar) |
        |  0                               0                    -1                     0 |

    Notes:
        • This is the standard OpenGL-style projection matrix.
        • Designed for column-major storage and column-vector multiplication.
        • The -1 in the last row (third column) performs the perspective divide.
        • After multiplication, clip-space coordinates are divided by w.
        • Depth range after projection is [-1, +1].
*/



/*
    Reverse‑Z orthographic projection matrix (Vulkan, column-major, column vectors).

    Parameters:
        left, right   – horizontal bounds of the view volume
        bottom, top   – vertical bounds of the view volume
        zNear         – distance to the near clipping plane (> 0)
        zFar          – distance to the far clipping plane (> zNear)

    Reverse‑Z behavior:
        • Near plane maps to depth = 1.0
        • Far plane maps to depth = 0.0
        • Depth test must use GREATER instead of LESS (if depth buffer is enabled)
        • Depth buffer should be floating-point (D32F) for best precision

    Mathematical form (Vulkan NDC Z ∈ [0, 1]):

        |  2 / (right - left)      0                        0                       0 |
        |  0                        2 / (top - bottom)      0                       0 |
        |  0                        0            1 / (zFar - zNear)   zNear / (zFar - zNear) |
        |  -(right + left)/(right - left)   -(top + bottom)/(top - bottom)    -1              1 |

    Notes:
        • Designed for column-major storage and column-vector multiplication: v' = P * v.
        • Produces NDC Z in range [1 (near), 0 (far)] — reverse-Z compatible.
        • Suitable for UI, 2D rendering, debug overlays, shadow maps, etc.
*/