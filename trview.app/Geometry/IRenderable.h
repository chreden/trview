#pragma once

#include <SimpleMath.h>
#include <trview.app/Geometry/ITransparencyBuffer.h>

namespace trview
{
    struct ICamera;

    /// Interface for something that can be rendered by the viewer.
    struct IRenderable
    {
        /// Destructor for IRenderable.
        virtual ~IRenderable() = 0;

        /// Render non-transparent faces of the object.
        /// @param camera The current camera to render with.
        /// @param colour The colour tint to use to render the object.
        virtual void render(const ICamera& camera, const DirectX::SimpleMath::Color& colour) = 0;

        /// Populate the transparency buffer with the transparent faces for the object.
        /// @param transparency The transparency buffer to populate.
        /// @param camera The current camera to render with.
        /// @param colour The colour tint to use for the triangles.
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) = 0;

        /// Get whether the object is visible.
        virtual bool visible() const = 0;

        /// Set whether the object is visible.
        virtual void set_visible(bool value) = 0;

        static const inline DirectX::SimpleMath::Color SelectionFill{ 0.0f, 0.0f, 0.0f, 0.999f };
    };
}
