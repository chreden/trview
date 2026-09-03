export module trview.app:ITrigger;

import std;
import std.compat;

import trview.common;
import trlevel;

import :IRenderable;
import :Triangle;
import :Types;
import :IFilterable;
import :ISector;

namespace trview
{
    struct ILevel;
    struct IRoom;
    struct PickResult;

    export struct ITrigger : public IRenderable, public IFilterable
    {
        const static inline Colour Trigger_Colour{ 0.5f, 1, 0, 1 };

        using Source = std::function<std::shared_ptr<ITrigger>(uint32_t, const std::weak_ptr<IRoom>&, uint16_t, uint16_t, const TriggerInfo&, trlevel::LevelVersion, const std::weak_ptr<ILevel>&, const std::weak_ptr<ISector>&)>;

        Event<> on_changed;

        virtual ~ITrigger() = 0;
        virtual uint32_t number() const = 0;
        virtual std::weak_ptr<IRoom> room() const = 0;
        virtual uint16_t x() const = 0;
        virtual uint16_t z() const = 0;
        virtual bool triggers_item(uint32_t index) const = 0;
        virtual TriggerType type() const = 0;
        virtual bool only_once() const = 0;
        virtual uint16_t flags() const = 0;
        virtual int16_t timer() const = 0;
        virtual uint16_t sector_id() const = 0;
        virtual Colour colour() const = 0;
        virtual std::vector<Command> commands() const = 0;
        virtual void set_colour(const std::optional<Colour>& colour) = 0;
        virtual void set_triangles(const std::vector<Triangle>& triangles) = 0;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
        virtual void set_position(const DirectX::SimpleMath::Vector3& position) = 0;
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        virtual std::weak_ptr<ILevel> level() const = 0;
        virtual std::weak_ptr<ISector> sector() const = 0;
    };


    /// Get the string representation of the trigger type specified.
    /// @param type The type to test.
    /// @returns The string version of the enum.
    export std::string to_string(TriggerType type);

    /// Get the string representation of the command type specified.
    /// @param type The type to test.
    /// @returns The string version of the enum.
    export std::string command_type_name(TriggerCommandType type);

    export bool has_command(const ITrigger& trigger, TriggerCommandType type);
    export bool has_any_command(const ITrigger& trigger, const std::vector<TriggerCommandType>& type);

    export bool command_has_index(TriggerCommandType type);
    export bool command_is_item(TriggerCommandType type);

    export uint32_t trigger_room(const std::shared_ptr<ITrigger>& trigger);
    export uint32_t trigger_room(const ITrigger& trigger);
}
