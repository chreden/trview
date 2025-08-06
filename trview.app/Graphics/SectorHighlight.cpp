#include "SectorHighlight.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const Color Highlight_Colour{ 1, 1, 0 };
    }

    ISectorHighlight::~ISectorHighlight()
    {
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

    void SectorHighlight::render(const ICamera& camera)
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

            std::vector<MeshVertex> vertices;
            std::vector<uint32_t> indices;

            for (const auto& t : triangles)
            {
                auto c = (t.v1 - t.v0).Cross(t.v2 - t.v0);
                c.Normalize();

                vertices.push_back({ t.v0 - c * 0.05f, Vector3::Down, { 0, 0 }, Highlight_Colour });
                vertices.push_back({ t.v1 - c * 0.05f, Vector3::Down, { 0, 0 }, Highlight_Colour });
                vertices.push_back({ t.v2 - c * 0.05f, Vector3::Down, { 0, 0 }, Highlight_Colour });
                indices.push_back(static_cast<uint32_t>(indices.size()));
                indices.push_back(static_cast<uint32_t>(indices.size()));
                indices.push_back(static_cast<uint32_t>(indices.size()));
            }

            _mesh = _mesh_source(vertices, std::vector<std::vector<uint32_t>>(), indices, std::vector<TransparentTriangle>(), std::vector<Triangle>(), {});
        }

        _mesh->render(_room_offset * camera.view_projection(), Color(1,1,1));
    }
}
