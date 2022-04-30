#include "SectorHighlight.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const Color Highlight_Colour{ 1, 1, 0 };
    }

    SectorHighlight::SectorHighlight(const IMesh::Source& mesh_source)
        : _mesh_source(mesh_source)
    {
    }

    void SectorHighlight::set_sector(const std::shared_ptr<ISector>& sector, const Matrix& room_offset)
    {
        _sector = sector;
        _room_offset = room_offset;
        _mesh.reset();
    }

    void SectorHighlight::render(const ICamera& camera, const ILevelTextureStorage& texture_storage)
    {
        if (!_sector)
        {
            return;
        }

        if (!_mesh)
        {
            auto triangles = _sector->triangles();
            if (triangles.empty())
            {
                return;
            }

            auto c1 = (triangles[0].v1 - triangles[0].v0).Cross(triangles[0].v2 - triangles[0].v0);
            auto c2 = (triangles[1].v1 - triangles[1].v0).Cross(triangles[1].v2 - triangles[1].v0);

            c1.Normalize();
            c2.Normalize();

            const std::vector<MeshVertex> vertices
            {
                { triangles[0].v0 - c1 * 0.05f, Vector3::Down, {0, 0}, Highlight_Colour },
                { triangles[0].v1 - c1 * 0.05f, Vector3::Down, {0, 0}, Highlight_Colour },
                { triangles[0].v2 - c1 * 0.05f, Vector3::Down, {0, 0}, Highlight_Colour },
                { triangles[1].v0 - c2 * 0.05f, Vector3::Down, {0, 0}, Highlight_Colour },
                { triangles[1].v1 - c2 * 0.05f, Vector3::Down, {0, 0}, Highlight_Colour },
                { triangles[1].v2 - c2 * 0.05f, Vector3::Down, {0, 0}, Highlight_Colour }
            };

            const std::vector<uint32_t> indices{ 0,  1,  2,  3,  4,  5, };
            _mesh = _mesh_source(vertices, std::vector<std::vector<uint32_t>>(), indices, std::vector<TransparentTriangle>(), std::vector<Triangle>());
        }

        _mesh->render(_room_offset * camera.view_projection(), texture_storage, Color(1,1,1));
    }
}
