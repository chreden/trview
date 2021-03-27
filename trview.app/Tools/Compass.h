#pragma once

#include <trview.graphics/Device.h>
#include <trview.graphics/RenderTarget.h>
#include <trview.graphics/Sprite.h>
#include <trview.app/Geometry/Mesh.h>
#include <trview.app/Camera/OrbitCamera.h>
#include "ICompass.h"

namespace trview
{
    struct ILevelTextureStorage;

    namespace graphics 
    {
        struct IShaderStorage;
    }

    /// The compass shows the X, Y and Z axes.
    class Compass final : public ICompass
    {
    public:
        /// Create a compass.
        /// @param device The device to use to render the compass.
        /// @param shader_storage The shader storage instance.
        Compass(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<graphics::IShaderStorage>& shader_storage);

        /// Render the compass.
        /// @param device The device to use to render the compass.
        /// @param camera The current camera being used to view the level.
        /// @param texture_storage The texture storage instance to use.
        void render(const graphics::IDevice& device, const ICamera& camera, const ILevelTextureStorage& texture_storage);

        /// Pick against the compass points.
        /// @param mouse_position The mouse position in screen space.
        /// @param screen_size The screen size.
        /// @param axis The axis that was hovered over.
        bool pick(const Point& mouse_position, const Size& screen_size, Axis& axis);

        /// Set whether the compass is visible.
        /// @param value Whether to render the compass.
        void set_visible(bool value);
    private:
        std::unique_ptr<graphics::RenderTarget> _render_target;
        std::unique_ptr<Mesh> _mesh;
        std::unique_ptr<graphics::Sprite> _sprite;
        OrbitCamera _mesh_camera;
        bool _visible{ true };
    };

    /// Get a string representation of a compass axis.
    /// @param axis The axis.
    /// @returns The name of the axis.
    std::wstring axis_name(Compass::Axis axis);

    /// Align a camera to a particular axis.
    /// @param camera The camera to adjust.
    /// @param axis The axis.
    void align_camera_to_axis(ICamera& camera, Compass::Axis axis);
}
