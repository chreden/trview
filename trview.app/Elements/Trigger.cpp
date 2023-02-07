#include "Trigger.h"
#include <trview.app/Elements/Types.h>
#include "IRoom.h"

namespace trview
{
    Command::Command(uint32_t number, TriggerCommandType type, uint16_t index)
        : _number(number), _type(type), _index(index)
    {
    }

    uint32_t Command::number() const
    {
        return _number;
    }

    TriggerCommandType Command::type() const
    {
        return _type;
    }

    uint16_t Command::index() const
    {
        return _index;
    }

    Trigger::Trigger(uint32_t number, const std::weak_ptr<IRoom>& room, uint16_t x, uint16_t z, const TriggerInfo& trigger_info, trlevel::LevelVersion level_version, const std::weak_ptr<ILevel>& level, const IMesh::TransparentSource& mesh_source)
        : _number(number), _room(room), _x(x), _z(z), _type(trigger_info.type), _only_once(trigger_info.oneshot), _flags(trigger_info.mask),
        _timer(level_version >= trlevel::LevelVersion::Tomb4 ? static_cast<int8_t>(trigger_info.timer) : trigger_info.timer), _sector_id(trigger_info.sector_id),
        _level_version(level_version), _mesh_source(mesh_source), _level(level)
    {
        uint32_t command_index = 0;
        for (auto action : trigger_info.commands)
        {
            _commands.push_back({ command_index++, action.first, action.second });
            // Special case for object still.
            if (action.first == TriggerCommandType::Object)
            {
                _objects.push_back(action.second);
            }
        }
    }

    uint32_t Trigger::number() const
    {
        return _number;
    }

    std::weak_ptr<IRoom> Trigger::room() const
    {
        return _room;
    }

    uint16_t Trigger::x() const
    {
        return _x;
    }

    uint16_t Trigger::z() const
    {
        return _z;
    }

    bool Trigger::triggers_item(uint32_t index) const
    {
        return std::find(_objects.begin(), _objects.end(), index) != _objects.end();
    }

    TriggerType Trigger::type() const
    {
        return _type;
    }

    bool Trigger::only_once() const
    {
        return _only_once;
    }

    uint16_t Trigger::flags() const
    {
        return _flags;
    }

    int16_t Trigger::timer() const
    {
        return _timer;
    }

    const std::vector<Command> Trigger::commands() const
    {
        return _commands;
    }

    uint16_t Trigger::sector_id() const
    {
        return _sector_id;
    }

    void Trigger::set_triangles(const std::vector<TransparentTriangle>& transparent_triangles)
    {
        std::vector<Triangle> collision;
        std::transform(transparent_triangles.begin(), transparent_triangles.end(), std::back_inserter(collision),
            [](const auto& tri) { return Triangle(tri.vertices[0], tri.vertices[1], tri.vertices[2]); });
        _mesh = _mesh_source(transparent_triangles, collision);
    }

    PickResult Trigger::pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
    {
        if (_mesh)
        {
            auto result = _mesh->pick(position, direction);
            if (result.hit)
            {
                result.type = PickResult::Type::Trigger;
                result.index = _number;
                return result;
            }
        }

        return PickResult();
    }

    void Trigger::render(const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&)
    {
    }

    void Trigger::get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera&, const DirectX::SimpleMath::Color& colour)
    {
        if (!_visible)
        {
            return;
        }

        using namespace DirectX::SimpleMath;
        for (auto& triangle : _mesh->transparent_triangles())
        {
            transparency.add(triangle.transform(Matrix::Identity, colour));
        }
    }

    void Trigger::set_position(const DirectX::SimpleMath::Vector3& position)
    {
        _position = position;
    }

    DirectX::SimpleMath::Vector3 Trigger::position() const
    {
        return _position;
    }

    bool Trigger::visible() const
    {
        return _visible;
    }

    void Trigger::set_visible(bool value)
    {
        _visible = value;
    }

    std::weak_ptr<ILevel> Trigger::level() const
    {
        return _level;
    }

    uint32_t trigger_room(const std::shared_ptr<ITrigger>& trigger)
    {
        if (!trigger)
        {
            return 0u;
        }
        return trigger_room(*trigger);
    }

    uint32_t trigger_room(const ITrigger& trigger)
    {
        if (auto room = trigger.room().lock())
        {
            return room->number();
        }
        return 0u;
    }
}
