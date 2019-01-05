#include "Waypoint.h"
#include "ICamera.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const float PoleThickness = 0.05f;
    }

    Waypoint::Waypoint(Mesh* mesh, const DirectX::SimpleMath::Vector3& position, uint32_t room)
        : _mesh(mesh), _position(position), _type(Type::Position), _index(0u), _room(room)
    {
    }

    Waypoint::Waypoint(Mesh* mesh, const DirectX::SimpleMath::Vector3& position, uint32_t room, Type type, uint32_t index)
        : _mesh(mesh), _position(position), _type(type), _index(index), _room(room)
    {
    }

    void Waypoint::render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, const Color& colour)
    {
        using namespace DirectX::SimpleMath;

        auto wvp = Matrix::CreateScale(PoleThickness, 0.5f, PoleThickness) * Matrix::CreateTranslation(_position - Vector3(0, 0.25f, 0)) * camera.view_projection();
        _mesh->render(device.context(), wvp, texture_storage, colour);
    }

    void Waypoint::get_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour)
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

    void Waypoint::set_notes(const std::wstring& notes)
    {
        _notes = notes;
    }
}

