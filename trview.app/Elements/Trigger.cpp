#include "Trigger.h"
#include <trview.app/Elements/Types.h>
#include <unordered_map>
#include <algorithm>
#include <trview.app/Geometry/TransparencyBuffer.h>

using namespace Microsoft::WRL;

namespace trview
{
    namespace
    {
        const std::unordered_map<TriggerType, std::wstring> trigger_type_names
        {
            { TriggerType::Trigger, L"Trigger" },
            { TriggerType::Pad, L"Pad" },
            { TriggerType::Switch, L"Switch" },
            { TriggerType::Key, L"Key" },
            { TriggerType::Pickup, L"Pickup" },
            { TriggerType::HeavyTrigger, L"Heavy Trigger" },
            { TriggerType::Antipad, L"Antipad" },
            { TriggerType::Combat, L"Combat" },
            { TriggerType::Dummy, L"Dummy" },
            { TriggerType::AntiTrigger, L"Antitrigger" },
            { TriggerType::HeavySwitch, L"Heavy Switch" },
            { TriggerType::HeavyAntiTrigger, L"Heavy Antitrigger" },
            { TriggerType::Monkey, L"Monkey" },
            { TriggerType::Skeleton, L"Skeleton" },
            { TriggerType::Tightrope, L"Tightrope" },
            { TriggerType::Crawl, L"Crawl" },
            { TriggerType::Climb, L"Climb"}
        };

        const std::unordered_map<TriggerCommandType, std::wstring> command_type_names
        {
            { TriggerCommandType::Object, L"Object" },
            { TriggerCommandType::Camera, L"Camera" },
            { TriggerCommandType::UnderwaterCurrent, L"Current" },
            { TriggerCommandType::FlipMap, L"Flip Map" },
            { TriggerCommandType::FlipOn, L"Flip On" },
            { TriggerCommandType::FlipOff, L"Flip Off" },
            { TriggerCommandType::LookAtItem, L"Look at Item" },
            { TriggerCommandType::EndLevel, L"End Level" },
            { TriggerCommandType::PlaySoundtrack, L"Music" },
            { TriggerCommandType::Flipeffect, L"Flipeffect" },
            { TriggerCommandType::SecretFound, L"Secret" },
            { TriggerCommandType::ClearBodies, L"Clear Bodies" },
            { TriggerCommandType::Flyby, L"Flyby" },
            { TriggerCommandType::Cutscene, L"Cutscene" }
        };

        const std::unordered_map<std::wstring, TriggerCommandType> command_type_lookup
        {
            { L"Object", TriggerCommandType::Object },
            { L"Camera", TriggerCommandType::Camera },
            { L"Current", TriggerCommandType::UnderwaterCurrent },
            { L"Flip Map", TriggerCommandType::FlipMap },
            { L"Flip On", TriggerCommandType::FlipOn },
            { L"Flip Off", TriggerCommandType::FlipOff },
            { L"Look at Item", TriggerCommandType::LookAtItem },
            { L"End Level", TriggerCommandType::EndLevel },
            { L"Music", TriggerCommandType::PlaySoundtrack },
            { L"Flipeffect", TriggerCommandType::Flipeffect },
            { L"Secret", TriggerCommandType::SecretFound },
            { L"Clear Bodies", TriggerCommandType::ClearBodies },
            { L"Flyby", TriggerCommandType::Flyby },
            { L"Cutscene", TriggerCommandType::Cutscene }
        };
    }

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

    Trigger::Trigger(uint32_t number, uint32_t room, uint16_t x, uint16_t z, const TriggerInfo& trigger_info)
        : _number(number), _room(room), _x(x), _z(z), _type(trigger_info.type), _only_once(trigger_info.oneshot), _flags(trigger_info.mask), _timer(trigger_info.timer), _sector_id(trigger_info.sector_id)
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

    uint32_t Trigger::room() const
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

    uint8_t Trigger::timer() const
    {
        return _timer;
    }

    const std::vector<Command>& Trigger::commands() const
    {
        return _commands;
    }

    uint16_t Trigger::sector_id() const
    {
        return _sector_id;
    }

    const std::vector<TransparentTriangle>& Trigger::triangles() const
    {
        return _mesh->transparent_triangles();
    }

    void Trigger::set_triangles(const std::vector<TransparentTriangle>& transparent_triangles)
    {
        std::vector<Triangle> collision;
        std::transform(transparent_triangles.begin(), transparent_triangles.end(), std::back_inserter(collision),
            [](const auto& tri) { return Triangle(tri.vertices[0], tri.vertices[1], tri.vertices[2]); });
        _mesh = std::make_unique<Mesh>(transparent_triangles, collision);
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

    bool Trigger::has_command(TriggerCommandType type) const
    {
        return std::any_of(_commands.begin(), _commands.end(), [&](const auto& c) { return c.type() == type; });
    }

    void Trigger::render(const graphics::Device&, const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&)
    {
    }

    void Trigger::get_transparent_triangles(TransparencyBuffer& transparency, const ICamera&, const DirectX::SimpleMath::Color& colour)
    {
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

    std::wstring trigger_type_name(TriggerType type)
    {
        auto name = trigger_type_names.find(type);
        if (name == trigger_type_names.end())
        {
            return L"Unknown";
        } 
        return name->second;
    }

    std::wstring command_type_name(TriggerCommandType type)
    {
        auto name = command_type_names.find(type);
        if (name == command_type_names.end())
        {
            return L"Unknown";
        }
        return name->second;
    }

    TriggerCommandType command_from_name(const std::wstring& name)
    {
        auto type = command_type_lookup.find(name);
        if (type == command_type_lookup.end())
        {
            return TriggerCommandType::Object;
        }
        return type->second;
    }
}
