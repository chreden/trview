#include "Waypoint.h"
#include <trview.app/Camera/ICamera.h>

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const float PoleThickness = 0.05f;
    }

    Waypoint::Waypoint(IMesh* mesh, const DirectX::SimpleMath::Vector3& position, uint32_t room)
        : _mesh(mesh), _position(position), _type(Type::Position), _index(0u), _room(room)
    {
    }

    Waypoint::Waypoint(IMesh* mesh, const DirectX::SimpleMath::Vector3& position, uint32_t room, Type type, uint32_t index, const Colour& route_colour)
        : _mesh(mesh), _position(position), _type(type), _index(index), _room(room), _route_colour(route_colour)
    {
    }

    void Waypoint::render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const Color& colour)
    {
        using namespace DirectX::SimpleMath;

        auto light_direction = _position - camera.position();
        light_direction.Normalize();

        // The pole
        auto pole_wvp = Matrix::CreateScale(PoleThickness, 0.5f, PoleThickness) * Matrix::CreateTranslation(_position - Vector3(0, 0.25f, 0)) * camera.view_projection();
        _mesh->render(pole_wvp, texture_storage, colour, light_direction);

        // The light blob.
        auto blob_wvp = Matrix::CreateScale(PoleThickness, PoleThickness, PoleThickness) * Matrix::CreateTranslation(_position - Vector3(0, 0.5f + PoleThickness * 0.5f, 0)) * camera.view_projection();
        _mesh->render(blob_wvp, texture_storage, _route_colour);
    }

    void Waypoint::get_transparent_triangles(ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&)
    {
    }

    DirectX::SimpleMath::Vector3 Waypoint::position() const
    {
        return _position;
    }

    Waypoint::Type Waypoint::type() const
    {
        return _type;
    }

    bool Waypoint::has_save() const
    {
        return !_save_data.empty();
    }

    uint32_t Waypoint::index() const
    {
        return _index;
    }

    uint32_t Waypoint::room() const
    {
        return _room;
    }

    std::wstring Waypoint::notes() const
    {
        return _notes;
    }

    std::vector<uint8_t> Waypoint::save_file() const
    {
        return _save_data;
    }

    void Waypoint::set_notes(const std::wstring& notes)
    {
        _notes = notes;
    }

    void Waypoint::set_route_colour(const Colour& colour)
    {
        _route_colour = colour;
    }

    void Waypoint::set_save_file(const std::vector<uint8_t>& data)
    {
        _save_data = data;
    }

    bool Waypoint::visible() const
    {
        return _visible;
    }

    void Waypoint::set_visible(bool value)
    {
        _visible = value;
    }
}

