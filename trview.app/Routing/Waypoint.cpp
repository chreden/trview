#include "Waypoint.h"
#include <trview.app/Camera/ICamera.h>
#include <trview.common/Maths.h>

#include "IRoute.h"
#include "../Elements/ILevel.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const float PoleLength = 0.5f;
        const float HalfPoleLength = 0.5f * PoleLength;
        const float PoleThickness = 0.05f;
        const float RopeThickness = 0.015f;
    }

    Waypoint::Waypoint(std::shared_ptr<IMesh> mesh, const Vector3& position, const Vector3& normal, uint32_t room, Type type, uint32_t index, const Colour& route_colour, const Colour& waypoint_colour)
        : _mesh(mesh), _position(position), _normal(normal), _type(type), _index(index), _room(room), _route_colour(route_colour), _waypoint_colour(waypoint_colour)
    {
    }

    void Waypoint::render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const Color& colour)
    {
        Matrix rotation = calculate_waypoint_rotation();

        // The pole
        const auto pole_world = Matrix::CreateScale(PoleThickness, PoleLength, PoleThickness) * Matrix::CreateTranslation(0, -HalfPoleLength, 0) * rotation * Matrix::CreateTranslation(_position);
        auto light_direction = Vector3::TransformNormal(_position - camera.position(), pole_world.Invert());
        light_direction.Normalize();

        auto pole_wvp = pole_world * camera.view_projection();
        _mesh->render(pole_wvp, texture_storage, colour, 0.75f, light_direction);

        // The light blob.
        auto blob_wvp = Matrix::CreateScale(PoleThickness, PoleThickness, PoleThickness) * Matrix::CreateTranslation(-Vector3(0, PoleLength + PoleThickness * 0.5f, 0)) * rotation * Matrix::CreateTranslation(_position) * camera.view_projection();
        _mesh->render(blob_wvp, texture_storage, colour == IRenderable::SelectionFill ? colour : static_cast<Color>(_route_colour));
    }

    void Waypoint::render_join(const IWaypoint& next_waypoint, const ICamera& camera, const ILevelTextureStorage& texture_storage, const Color& colour)
    {
        const auto current = blob_position();
        const auto next_waypoint_pos = next_waypoint.blob_position();
        const auto mid = Vector3::Lerp(current, next_waypoint_pos, 0.5f);
        const auto to = next_waypoint_pos - current;
        const auto matrix =
            (to.x == 0 && to.z == 0)
            ? Matrix::CreateRotationX(maths::Pi * 0.5f) * Matrix::CreateTranslation(mid)
            : Matrix(XMMatrixLookAtRH(mid, next_waypoint_pos, Vector3::Up)).Invert();
        const auto length = to.Length();
        const auto to_wvp = Matrix::CreateScale(RopeThickness, RopeThickness, length) * matrix * camera.view_projection();
        _mesh->render(to_wvp, texture_storage, colour);
    }

    void Waypoint::get_transparent_triangles(ITransparencyBuffer&, const ICamera&, const Color&)
    {
    }

    BoundingBox Waypoint::bounding_box() const
    {
        const float length = PoleLength + PoleThickness;
        BoundingBox box(Vector3(0, -0.5f * length, 0), Vector3(PoleThickness, length, PoleThickness) * 0.5f);
        Matrix transform = calculate_waypoint_rotation() * Matrix::CreateTranslation(position());
        box.Transform(box, transform);
        return box;
    }

    Vector3 Waypoint::position() const
    {
        return _position;
    }

    Vector3 Waypoint::blob_position() const
    {
        auto matrix = Matrix::CreateTranslation(-Vector3(0, PoleLength + PoleThickness * 0.5f, 0)) * calculate_waypoint_rotation() * Matrix::CreateTranslation(_position);
        return Vector3::Transform(Vector3(), matrix);
    }

    Matrix Waypoint::calculate_waypoint_rotation() const
    {
        Matrix rotation = Matrix(XMMatrixLookAtRH(Vector3::Zero, _normal, Vector3::Up)).Invert();
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

    IWaypoint::Type Waypoint::type() const
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

    std::weak_ptr<IItem> Waypoint::item() const
    {
        if (auto existing = _item.lock())
        {
            return existing;
        }

        if (type() == IWaypoint::Type::Entity)
        {
            if (auto route = _route.lock())
            {
                if (auto level = route->level().lock())
                {
                    _item = level->item(index());
                }
            }
        }
        return _item;
    }

    uint32_t Waypoint::room() const
    {
        return _room;
    }

    std::string Waypoint::notes() const
    {
        return _notes;
    }

    std::weak_ptr<IRoute> Waypoint::route() const
    {
        return _route;
    }

    std::vector<uint8_t> Waypoint::save_file() const
    {
        return _save_data;
    }

    void Waypoint::set_item(const std::weak_ptr<IItem>& item)
    {
        _item = item;
        if (auto new_item = _item.lock())
        {
            set_properties(Type::Entity, new_item->number(), item_room(new_item), new_item->position());
        }
    }

    void Waypoint::set_notes(const std::string& notes)
    {
        _notes = notes;
    }

    void Waypoint::set_route(const std::weak_ptr<IRoute>& route)
    {
        _route = route;
    }

    void Waypoint::set_route_colour(const Colour& colour)
    {
        _route_colour = colour;
    }

    void Waypoint::set_save_file(const std::vector<uint8_t>& data)
    {
        _save_data = data;
    }

    void Waypoint::set_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        _trigger = trigger;
        if (auto new_trigger = _trigger.lock())
        {
            set_properties(Type::Trigger, new_trigger->number(), new_trigger->room(), new_trigger->position());
        }
    }

    void Waypoint::set_waypoint_colour(const Colour& colour)
    {
        _waypoint_colour = colour;
    }

    bool Waypoint::visible() const
    {
        return _visible;
    }

    void Waypoint::set_visible(bool value)
    {
        _visible = value;
    }

    Vector3 Waypoint::normal() const
    {
        return _normal;
    }

    IWaypoint::WaypointRandomizerSettings Waypoint::randomizer_settings() const
    {
        return _randomizer_settings;
    }

    void Waypoint::set_randomizer_settings(const WaypointRandomizerSettings& settings)
    {
        _randomizer_settings = settings;
    }

    void Waypoint::set_position(const DirectX::SimpleMath::Vector3& position)
    {
        set_properties(Type::Position, 0, 0, position);
    }

    std::weak_ptr<ITrigger> Waypoint::trigger() const
    {
        if (auto existing = _trigger.lock())
        {
            return existing;
        }

        if (type() == IWaypoint::Type::Trigger)
        {
            if (auto route = _route.lock())
            {
                if (auto level = route->level().lock())
                {
                    _trigger = level->trigger(index());
                }
            }
        }
        return _trigger;
    }

    Colour Waypoint::route_colour() const
    {
        return _route_colour;
    }

    Colour Waypoint::waypoint_colour() const
    {
        return _waypoint_colour;
    }

    void Waypoint::set_properties(Type type, uint32_t index, uint32_t room, const DirectX::SimpleMath::Vector3& position)
    {
        _type = type;
        _index = index;
        _room = room;
        _position = position;
        on_changed();
    }

    void Waypoint::set_normal(const DirectX::SimpleMath::Vector3& normal)
    {
        _normal = normal;
        on_changed();
    }

    void Waypoint::set_room_number(uint32_t room)
    {
        _room = room;
        on_changed();
    }
}

