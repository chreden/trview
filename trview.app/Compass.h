#pragma once

#include <trview.graphics/Device.h>
#include <trview.graphics/RenderTarget.h>
#include <trview.graphics/Sprite.h>
#include "Mesh.h"
#include "Camera.h"

namespace trview
{
    struct ILevelTextureStorage;

    namespace graphics 
    {
        struct IShaderStorage;
    }

    /// The compass shows the X, Y and Z axes.
    class Compass final
    {
    public:
        /// Create a compass.
        /// @param device The device to use to render the compass.
        /// @param shader_storage The shader storage instance.
        Compass(const graphics::Device& device, const graphics::IShaderStorage& shader_storage);

        /// Render the compass.
        /// @param device The device to use to render the compass.
        /// @param camera The current camera being used to view the level.
        /// @param texture_storage The texture storage instance to use.
        void render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage);
    private:
        std::unique_ptr<graphics::RenderTarget> _render_target;
        std::unique_ptr<Mesh> _mesh;
        std::unique_ptr<graphics::Sprite> _sprite;
        Camera _mesh_camera;
    };
}
