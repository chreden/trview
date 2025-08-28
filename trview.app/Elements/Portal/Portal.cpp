#include "Portal.h"
#include "../../Geometry/ITransparencyBuffer.h"

namespace trview
{
    IPortal::~IPortal()
    {
    }

    Portal::Portal(const trlevel::tr_room_portal& portal, DirectX::SimpleMath::Vector3 room_offset)
        : _room(portal.adjoining_room), _normal(portal.normal.x, portal.normal.y, portal.normal.z), _room_offset(room_offset)
    {
        _vertices =
        {
            (DirectX::SimpleMath::Vector3(portal.vertices[0].x, portal.vertices[0].y, portal.vertices[0].z) / trlevel::Scale),
            (DirectX::SimpleMath::Vector3(portal.vertices[1].x, portal.vertices[1].y, portal.vertices[1].z) / trlevel::Scale),
            (DirectX::SimpleMath::Vector3(portal.vertices[2].x, portal.vertices[2].y, portal.vertices[2].z) / trlevel::Scale),
            (DirectX::SimpleMath::Vector3(portal.vertices[3].x, portal.vertices[3].y, portal.vertices[3].z) / trlevel::Scale)
        };
    }

    void Portal::get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera&)
    {
        const DirectX::SimpleMath::Color portal_colour = DirectX::SimpleMath::Color(1.0f, 0.0f, 0.0f, 0.5f);
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

    DirectX::SimpleMath::Vector3 Portal::normal() const
    {
        return _normal;
    }

    uint16_t Portal::room() const
    {
        return _room;
    }

    std::vector<DirectX::SimpleMath::Vector3> Portal::vertices() const
    {
        return _vertices;
    }
}
