#include "Waypoint.h"
#include <trview.app/Camera/ICamera.h>

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const float PoleThickness = 0.005f;
    }

    Waypoint::Waypoint(Mesh* mesh, const DirectX::SimpleMath::Vector3& position, uint32_t room)
        : _mesh(mesh), _position(position), _type(Type::Position), _index(0u), _room(room)
    {
    }

    Waypoint::Waypoint(Mesh* mesh, const DirectX::SimpleMath::Vector3& position, uint32_t room, Type type, uint32_t index, const Colour& route_colour)
        : _mesh(mesh), _position(position), _type(type), _index(index), _room(room), _route_colour(route_colour)
    {
    }

    void Waypoint::render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, const Color& colour)
    {
        using namespace DirectX::SimpleMath;

        auto light_direction = _position - camera.position();
        light_direction.Normalize();

        // The pole
        auto pole_wvp = Matrix::CreateScale(PoleThickness, 0.5f, PoleThickness) * Matrix::CreateTranslation(_position - Vector3(0, 0.25f, 0)) * camera.view_projection();
        // _mesh->render(device.context(), pole_wvp, texture_storage, colour, light_direction);

        // The light blob.
        // auto blob_wvp = Matrix::CreateScale(PoleThickness, PoleThickness, PoleThickness) * Matrix::CreateTranslation(_position - Vector3(0, 0.5f + PoleThickness * 0.5f, 0)) * camera.view_projection();
        auto blob_wvp = Matrix::CreateScale(PoleThickness, PoleThickness, PoleThickness) * Matrix::CreateTranslation(_position) * camera.view_projection();
        _mesh->render(device.context(), blob_wvp, texture_storage, _route_colour);
    }

    void Waypoint::get_transparent_triangles(TransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&)
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

    Waypoint::Type waypoint_type_from_string(const std::string& value)
    {
        if (value == "Trigger")
        {
            return Waypoint::Type::Trigger;
        }

        if (value == "Entity")
        {
            return Waypoint::Type::Entity;
        }

        return Waypoint::Type::Position;
    }

    std::wstring waypoint_type_to_string(Waypoint::Type type)
    {
        switch (type)
        {
        case Waypoint::Type::Entity:
            return L"Entity";
        case Waypoint::Type::Position:
            return L"Position";
        case Waypoint::Type::Trigger:
            return L"Trigger";
        }
        return L"Unknown";
    }
}

