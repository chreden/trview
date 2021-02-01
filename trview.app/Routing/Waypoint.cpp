#include "Waypoint.h"
#include <trview.app/Camera/ICamera.h>
#include <trview.common/Maths.h>

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const float PoleThickness = 0.05f;
    }

    Waypoint::Waypoint(Mesh* mesh, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room)
        : _mesh(mesh), _position(position), _normal(normal), _type(Type::Position), _index(0u), _room(room)
    {
    }

    Waypoint::Waypoint(Mesh* mesh, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, Type type, uint32_t index, const Colour& route_colour)
        : _mesh(mesh), _position(position), _normal(normal), _type(type), _index(index), _room(room), _route_colour(route_colour)
    {
    }

    void Waypoint::render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, const Color& colour)
    {
        using namespace DirectX::SimpleMath;

        auto light_direction = _position - camera.position();
        light_direction.Normalize();

        Matrix rotation = calculate_waypoint_rotation();

        // The pole
        auto pole_wvp = Matrix::CreateScale(PoleThickness, 0.5f, PoleThickness) * Matrix::CreateTranslation(0, -0.25f, 0) * rotation * Matrix::CreateTranslation(_position) * camera.view_projection();
        _mesh->render(device.context(), pole_wvp, texture_storage, colour, light_direction);

        // The light blob.
        auto blob_wvp = Matrix::CreateScale(PoleThickness, PoleThickness, PoleThickness) * Matrix::CreateTranslation(-Vector3(0, 0.5f + PoleThickness * 0.5f, 0)) * rotation * Matrix::CreateTranslation(_position) * camera.view_projection();
        _mesh->render(device.context(), blob_wvp, texture_storage, _route_colour);
    }

    void Waypoint::get_transparent_triangles(TransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&)
    {
    }

    DirectX::SimpleMath::Vector3 Waypoint::position() const
    {
        return _position;
    }

    DirectX::SimpleMath::Vector3 Waypoint::blob_position() const
    {
        auto matrix = Matrix::CreateTranslation(-Vector3(0, 0.5f + PoleThickness * 0.5f, 0)) * calculate_waypoint_rotation() * Matrix::CreateTranslation(_position);
        return Vector3::Transform(Vector3(), matrix);
    }

    Matrix Waypoint::calculate_waypoint_rotation() const
    {
        Matrix rotation = Matrix(DirectX::XMMatrixLookAtRH(Vector3::Zero, _normal, Vector3::Up)).Invert();
        rotation = Matrix::CreateRotationX(maths::Pi * 0.5f) * rotation;

        if (_normal == Vector3::Down)
        {
            rotation = Matrix::Identity;
        }
        else if (_normal == Vector3::Up)
        {
            rotation = Matrix::CreateRotationX(maths::Pi);
        }
        return rotation;
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

