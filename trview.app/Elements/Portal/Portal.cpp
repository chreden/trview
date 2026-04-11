#include "Portal.h"
#include "../../Geometry/ITransparencyBuffer.h"
#include "../ISector.h"
#include "../IRoom.h"

namespace trview
{
    using namespace DirectX::SimpleMath;

    namespace
    {
        void add_rect(std::vector<Triangle>& triangles, const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& normal)
        {
            constexpr Vector2 uv0 = { 0, 0 };
            constexpr Vector2 uv1 = { 1, 0 };
            constexpr Vector2 uv2 = { 1, 1 };
            constexpr Vector2 uv3 = { 0, 1 };
            const Color color = Colour::White;

            triangles.push_back(
                Triangle
                {
                    .collision_mode = Triangle::CollisionMode::Enabled,
                    .colours = { color, color, color },
                    .frames = { {.uvs = { uv0, uv1, uv2 } } },
                    .normals = { normal, normal, normal },
                    .texture_mode = Triangle::TextureMode::Untextured,
                    .transparency_mode = Triangle::TransparencyMode::Normal,
                    .vertices = { v0, v1, v2 }
                });
            triangles.push_back(
                Triangle
                {
                    .collision_mode = Triangle::CollisionMode::Enabled,
                    .colours = { color, color, color },
                    .frames = { {.uvs = { uv2, uv3, uv0 } } },
                    .normals = { normal, normal, normal },
                    .texture_mode = Triangle::TextureMode::Untextured,
                    .transparency_mode = Triangle::TransparencyMode::Normal,
                    .vertices = { v2, v3, v0 }
                });
        }
    }

    IPortal::~IPortal()
    {
    }

    Portal::Portal(const trlevel::tr_room_portal& portal, Vector3 room_offset)
        : _room(portal.adjoining_room), _normal(portal.normal.x, portal.normal.y, portal.normal.z), _room_offset(room_offset)
    {
        _vertices =
        {
            Vector3(portal.vertices[0].x, portal.vertices[0].y, portal.vertices[0].z) / trlevel::Scale),
            Vector3(portal.vertices[1].x, portal.vertices[1].y, portal.vertices[1].z) / trlevel::Scale),
            Vector3(portal.vertices[2].x, portal.vertices[2].y, portal.vertices[2].z) / trlevel::Scale),
            Vector3(portal.vertices[3].x, portal.vertices[3].y, portal.vertices[3].z) / trlevel::Scale)
        };
    }

    Portal::Portal(const std::shared_ptr<ISector>& sector, uint16_t room, const IMesh::Source& mesh_source)
        : _room(room)
    {
        _vertices.resize(4);

        auto y_bottom = sector->corners();

        const auto info = sector->room().lock()->info();

        // Figure out if we should make the walls based on adjacent triggers.
        bool pos_x = true, neg_x = true, pos_z = true, neg_z = true;

        // Calculate the X/Z position.
        const float x = info.x / trlevel::Scale_X + sector->x() + 0.5f;
        const float z = info.z / trlevel::Scale_Z + sector->z() + 0.5f;
        const float height = 0.25f;

        std::array<float, 4> y_top = { 0,0,0,0 };
        for (int i = 0; i < 4; ++i)
        {
            y_top[i] = y_bottom[i] - height;
        }

        const std::array<Vector3, 4> t
        {
            Vector3(x - 0.5f, y_top[0], z - 0.5f),
            Vector3(x - 0.5f, y_top[1], z + 0.5f),
            Vector3(x + 0.5f, y_top[2], z - 0.5f),
            Vector3(x + 0.5f, y_top[3], z + 0.5f)
        };
        const std::array<Vector3, 4> b
        {
            Vector3(x - 0.5f, y_bottom[0], z - 0.5f),
            Vector3(x - 0.5f, y_bottom[1], z + 0.5f),
            Vector3(x + 0.5f, y_bottom[2], z - 0.5f),
            Vector3(x + 0.5f, y_bottom[3], z + 0.5f)
        };

        std::vector<Triangle> triangles;
        const Color colour = ITrigger::Trigger_Colour;

        const auto add_tri = [&triangles, &colour](const Vector3& v0, const Vector3& v1, const Vector3& v2)
            {
                const auto normal = (v2 - v1).Cross(v1 - v0);
                triangles.push_back(
                    Triangle
                    {
                        .colours = { colour, colour, colour },
                        .normals { normal, normal, normal },
                        .texture_mode = Triangle::TextureMode::Untextured,
                        .transparency_mode = Triangle::TransparencyMode::Normal,
                        .vertices = { v0, v1, v2 }
                    });
            };

        // + Y
        if (sector->triangulation() == TriangulationDirection::NwSe)
        {
            add_tri(t[3], t[2], t[1]);
            add_tri(t[0], t[1], t[2]);
        }
        else
        {
            add_tri(t[3], t[2], t[0]);
            add_tri(t[0], t[1], t[3]);
        }

        if (pos_x)
        {
            // + X
            add_tri(t[2], t[3], b[3]);
            add_tri(t[2], b[3], b[2]);
        }

        if (neg_x)
        {
            // - X
            add_tri(t[1], t[0], b[0]);
            add_tri(t[1], b[0], b[1]);
        }

        if (pos_z)
        {
            // + Z
            add_tri(t[3], t[1], b[1]);
            add_tri(t[3], b[1], b[3]);
        }

        if (neg_z)
        {
            // - Z
            add_tri(t[0], t[2], b[2]);
            add_tri(t[0], b[2], b[0]);
        }

        _mesh = mesh_source(triangles);
    }

    void Portal::get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera&)
    {
        if (_mesh)
        {
            for (auto& triangle : _mesh->transparent_triangles())
            {
                transparency.add(triangle.transform(Matrix::Identity, Colour(0.5f, 0.0f, 1.0f, 0.0f), true));
            }
            return;
        }

        const Color portal_colour = Color(1.0f, 0.0f, 0.0f, 0.5f);
        transparency.add({
            .colours = { portal_colour, portal_colour, portal_colour },
            .texture_mode = Triangle::TextureMode::Untextured,
            .transparency_mode = Triangle::TransparencyMode::Normal,
            .vertices = { _vertices[0] + _room_offset, _vertices[1] + _room_offset, _vertices[2] + _room_offset } });
        transparency.add({
            .colours = { portal_colour, portal_colour, portal_colour },
            .texture_mode = Triangle::TextureMode::Untextured,
            .transparency_mode = Triangle::TransparencyMode::Normal,
            .vertices = { _vertices[2] + _room_offset, _vertices[3] + _room_offset, _vertices[0] + _room_offset } });
    }

    Vector3 Portal::normal() const
    {
        return _normal;
    }

    uint16_t Portal::room() const
    {
        return _room;
    }

    std::vector<Vector3> Portal::vertices() const
    {
        return _vertices;
    }
}
