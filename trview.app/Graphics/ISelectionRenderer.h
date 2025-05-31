#pragma once

#include <trview.app/Camera/ICamera.h>
#include <trview.app/Geometry/IRenderable.h>

namespace trview
{
    struct ISelectionRenderer
    {
        virtual ~ISelectionRenderer() = 0;

        /// Render the outline around the specified object.
        /// @param camera The current camera.
        /// @param selected_item The entity to outline.
        /// @param outline_colour The outline colour
        virtual void render(const ICamera& camera, IRenderable& selected_item, const DirectX::SimpleMath::Color& outline_colour) = 0;
    };
}