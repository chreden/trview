#pragma once

#include "Triangle.h"
#include <trview.app/Camera/ICamera.h>

namespace trview
{
    struct ILevelTextureStorage;
    struct ITransparencyBuffer
    {
        using Source = std::function<std::unique_ptr<ITransparencyBuffer>(const std::weak_ptr<ILevelTextureStorage>&)>;

        virtual ~ITransparencyBuffer() = 0;
        // Add a triangle to the transparency buffer. The triangle will be added to the end
        // of the collection.
        // triangle: The triangle to add.
        virtual void add(const Triangle& triangle) = 0;

        // Sort the accumulated transparent triangles in order of farthest to
        // nearest, based on the position of the camera.
        // eye_position: The position of the camera.
        virtual void sort(const DirectX::SimpleMath::Vector3& eye_position) = 0;

        /// Render the accumulated transparent triangles. Sort should be called before this function is called.
        /// @param view_project The view projection.
        /// @param texture_storage Texture storage for the level.
        /// @param ignore_blend Optional. Set to true to render this without transparency.
        virtual void render(const DirectX::SimpleMath::Matrix& view_projection, bool ignore_blend = false) = 0;

        // Reset the triangles buffer.
        virtual void reset() = 0;
    };
}
