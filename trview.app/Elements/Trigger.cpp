#include "Trigger.h"
#include <trview.app/Elements/Types.h>
#include <ranges>

namespace trview
{
    Command::Command(uint32_t number, TriggerCommandType type, const std::vector<uint16_t>& data)
        : _number(number), _type(type), _data(data)
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
        return _data.empty() ? 0 : _data[0];
    }

    std::vector<uint16_t> Command::data() const
    {
        return _data;
    }

    Trigger::Trigger(uint32_t number, const std::weak_ptr<IRoom>& room, uint16_t x, uint16_t z, const TriggerInfo& trigger_info, trlevel::LevelVersion level_version, const std::weak_ptr<ILevel>& level, const std::weak_ptr<ISector>& sector, const IMesh::Source& mesh_source)
        : _number(number), _room(room), _x(x), _z(z), _type(trigger_info.type), _only_once(trigger_info.oneshot), _flags(trigger_info.mask),
        _timer(level_version >= trlevel::LevelVersion::Tomb4 ? static_cast<int8_t>(trigger_info.timer) : trigger_info.timer), _sector_id(trigger_info.sector_id),
        _level_version(level_version), _mesh_source(mesh_source), _level(level), _sector(sector)
    {
        uint32_t command_index = 0;
        for (const auto& action : trigger_info.commands)
        {
            const Command command{ command_index++, action.type, action.data };
            if (equals_any(action.type, TriggerCommandType::Object, TriggerCommandType::LookAtItem))
            {
                _objects.push_back(command.index());
            }
            _commands.push_back(command);
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

    std::vector<Command> Trigger::commands() const
    {
        return _commands;
    }

    uint16_t Trigger::sector_id() const
    {
        return _sector_id;
    }

    Colour Trigger::colour() const
    {
        return _colour.value_or(Trigger_Colour);
    }

    void Trigger::set_colour(const std::optional<Colour>& colour)
    {
        _colour = colour;
        on_changed();
    }

    void Trigger::set_triangles(const std::vector<Triangle>& transparent_triangles)
    {
        _mesh = _mesh_source(transparent_triangles);
    }

    PickResult Trigger::pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
    {
        if (_mesh)
        {
            auto result = _mesh->pick(position, direction);
            if (result.hit)
            {
                result.type = PickResult::Type::Trigger;
                result.trigger = std::const_pointer_cast<ITrigger>(shared_from_this());
                return result;
            }
        }

        return PickResult();
    }

    void Trigger::render(const ICamera&, const DirectX::SimpleMath::Color&)
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
            transparency.add(triangle.transform(Matrix::Identity, colour, true));
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
        on_changed();
    }

    std::weak_ptr<ILevel> Trigger::level() const
    {
        return _level;
    }

    std::weak_ptr<ISector> Trigger::sector() const
    {
        return _sector;
    }

    int32_t Trigger::filterable_index() const
    {
        return static_cast<int32_t>(number());
    }
}
