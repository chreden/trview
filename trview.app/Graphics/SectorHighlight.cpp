#include "SectorHighlight.h"
#include <ranges>

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

            std::vector<UniTriangle> out_triangles = triangles |
                std::views::transform([](auto&& t) -> UniTriangle
                    {
                        auto c = (t.v1 - t.v0).Cross(t.v2 - t.v0);
                        c.Normalize();
                        return
                        {
                            .collision_mode = UniTriangle::CollisionMode::Disabled,
                            .colours = { Highlight_Colour, Highlight_Colour, Highlight_Colour },
                            .normals = { Vector3::Down, Vector3::Down, Vector3::Down },
                            .texture_mode = UniTriangle::TextureMode::Untextured,
                            .vertices = { t.v0 - c * 0.05f, t.v1 - c * 0.05f, t.v2 - c * 0.05f }
                        };
                    }) | std::ranges::to<std::vector>();

            _mesh = _mesh_source({}, {}, {}, {}, {}, {}, out_triangles);
        }

        _mesh->render(_room_offset * camera.view_projection(), Color(1,1,1));
    }
}
