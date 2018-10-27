#include "Compass.h"
#include "ICamera.h"
#include "MeshVertex.h"
#include <trview.graphics/Sprite.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/RenderTargetStore.h>
#include <trview.graphics/ViewportStore.h>

namespace trview
{
    using namespace graphics;
    using namespace DirectX::SimpleMath;

    namespace
    {
        const float Pi = 1.5707963267948966192313216916398f;
        const float View_Size = 200;
    }

    Compass::Compass()
        : _mesh_camera(Size(View_Size, View_Size))
    {
    }

    void Compass::render(const Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, const IShaderStorage& shader_storage)
    {
        auto context = device.context();

        // The process for rendering the compass:
        // Select an area to render the mesh to - let's say a 100x100 square in the bottom right
        // Create a render target
        if (!_render_target)
        {
            _render_target = std::make_unique<RenderTarget>(device.device(), View_Size, View_Size, RenderTarget::DepthStencilMode::Enabled);
        }

        {
            RenderTargetStore rs_store(context);
            ViewportStore vp_store(context);
            _render_target->apply(context);
            _render_target->clear(context, Color(0.0f, 0.0f, 0.0f, 0.0f));

            // Have a camera that looks at the compass
            // Match rotation to the real camera
            _mesh_camera.set_target(Vector3::Zero);
            _mesh_camera.set_zoom(2.5f);
            _mesh_camera.set_rotation_pitch(camera.rotation_pitch());
            _mesh_camera.set_rotation_yaw(camera.rotation_yaw());

            // Render the mesh 
            // Render the axes mesh 
            if (!_mesh)
            {
                _mesh = create_cube_mesh(device.device());
            }

            const auto view_projection = _mesh_camera.view_projection();
            const auto scale = Matrix::CreateScale(0.015f, 1.0f, 0.015f);

            // Y
            _mesh->render(context, scale * view_projection, texture_storage, Color(1.0f, 0.0f, 0.0f));
            // X
            _mesh->render(context, scale * Matrix::CreateRotationZ(Pi) * view_projection, texture_storage, Color(0.0f, 1.0f, 0.0f));
            // Z
            _mesh->render(context, scale * Matrix::CreateRotationX(Pi) * view_projection, texture_storage, Color(0.0f, 0.0f, 1.0f));
        }

        // Render the image to the screen somewhere.
        auto sprite = std::make_unique<Sprite>(device.device(), shader_storage, camera.view_size());
        auto screen_size = camera.view_size();
        sprite->render(context, _render_target->texture(), screen_size.width - View_Size, screen_size.height - View_Size, View_Size, View_Size);
    }
}
