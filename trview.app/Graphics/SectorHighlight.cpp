#include "SectorHighlight.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const Color Highlight_Colour{ 1, 1, 0 };
    }

    void SectorHighlight::set_sector(const std::shared_ptr<Sector>& sector, const Matrix& room_offset)
    {
        _sector = sector;
        _room_offset = room_offset;
        _mesh.reset();
    }

    void SectorHighlight::render(graphics::IDevice& device, const ICamera& camera, const ILevelTextureStorage& texture_storage)
    {
        if (!_sector)
        {
            return;
        }

        if (!_mesh)
        {
            auto triangles = _sector->triangles();
            auto c1 = (triangles[1] - triangles[0]).Cross(triangles[2] - triangles[0]);
            auto c2 = (triangles[4] - triangles[3]).Cross(triangles[5] - triangles[3]);

            c1.Normalize();
            c2.Normalize();

            const std::vector<MeshVertex> vertices
            {
                { triangles[0] - c1 * 0.05f, Vector3::Down, {0, 0}, Highlight_Colour },
                { triangles[1] - c1 * 0.05f, Vector3::Down, {0, 0}, Highlight_Colour },
                { triangles[2] - c1 * 0.05f, Vector3::Down, {0, 0}, Highlight_Colour },
                { triangles[3] - c2 * 0.05f, Vector3::Down, {0, 0}, Highlight_Colour },
                { triangles[4] - c2 * 0.05f, Vector3::Down, {0, 0}, Highlight_Colour },
                { triangles[5] - c2 * 0.05f, Vector3::Down, {0, 0}, Highlight_Colour }
            };

            const std::vector<uint32_t> indices{ 0,  1,  2,  3,  4,  5, };
            // TODO: Use DI
            _mesh = std::make_unique<Mesh>(device, vertices, std::vector<std::vector<uint32_t>>(), indices, std::vector<TransparentTriangle>(), std::vector<Triangle>());
        }

        _mesh->render(device.context(), _room_offset * camera.view_projection(), texture_storage, Color(1,1,1));
    }
}
