#include "Compass.h"
#include "ICamera.h"
#include "MeshVertex.h"

namespace trview
{
    using namespace graphics;
    using namespace DirectX::SimpleMath;

    void Compass::render(const Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage)
    {
        auto context = device.context();

        // The process for rendering the compass:
        // Select an area to render the mesh to - let's say a 100x100 square in the bottom right
        // Create a render target
        if (!_render_target)
        {
            _render_target = std::make_unique<RenderTarget>(device.device(), 100, 100, RenderTarget::DepthStencilMode::Enabled);
        }

        // _render_target->apply(context);

        // Have a camera that looks at the compass
        // Match rotation to the real camera

        // Render the mesh 
        // Render the axes mesh 
        if (!_mesh)
        {
            const std::vector<MeshVertex> vertices
            {
                { { 0.5f, 0.5f, 0.5f },  { 0, 0 }, { 1.0f, 1.0f, 1.0f } },
                { { 0.5f, 0.5f, -0.5f }, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },
                { { -0.5f, 0.5f, -0.5f }, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },
                { { -0.5f, 0.5f, 0.5f }, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },
                { { 0.0f, -0.5f, 0.0f }, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },
            };

            const std::vector<uint32_t> indices
            {
                0, 4, 1, // + x
                2, 4, 3, // - x
                0, 3, 4, // + z
                1, 4, 2, // - z
                0, 1, 2, 2, 3, 0  // - y
            };

            _mesh = std::make_unique<Mesh>(device.device(), vertices, std::vector<std::vector<uint32_t>>(), indices, std::vector<TransparentTriangle>(), std::vector<Triangle>());
        }

        auto wvp = camera.view_projection();
        _mesh->render(context, wvp, texture_storage, Color(1.0f, 0.0f, 0.0f));
    }
}
