#pragma once

#include <cstdint>
#include <vector>
#include <trview.app/Geometry/IRenderable.h>
#include <trview.app/Geometry/TransparentTriangle.h>
#include <trview.app/Geometry/PickResult.h>
#include <trview.app/Elements/Types.h>

namespace trview
{
    struct ITrigger : public IRenderable
    {
        using Source = std::function<std::shared_ptr<ITrigger>(uint32_t, uint32_t, uint16_t, uint16_t, const TriggerInfo&)>;

        virtual ~ITrigger() = 0;
        virtual uint32_t number() const = 0;
        virtual uint32_t room() const = 0;
        virtual uint16_t x() const = 0;
        virtual uint16_t z() const = 0;
        virtual bool triggers_item(uint32_t index) const = 0;
        virtual TriggerType type() const = 0;
        virtual bool only_once() const = 0;
        virtual uint16_t flags() const = 0;
        virtual uint8_t timer() const = 0;
        virtual uint16_t sector_id() const = 0;
        virtual const std::vector<Command> commands() const = 0;
        virtual void set_triangles(const std::vector<TransparentTriangle>& transparent_triangles) = 0;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
        virtual void set_position(const DirectX::SimpleMath::Vector3& position) = 0;
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
    };


    /// Get the string representation of the trigger type specified.
    /// @param type The type to test.
    /// @returns The string version of the enum.
    std::wstring trigger_type_name(TriggerType type);

    /// Get the string representation of the command type specified.
    /// @param type The type to test.
    /// @returns The string version of the enum.
    std::wstring command_type_name(TriggerCommandType type);

    /// Get the trigger command type from a string.
    /// @param name The string to convert.
    /// @returns The trigger command type.
    TriggerCommandType command_from_name(const std::wstring& name);

    bool has_command(const ITrigger& trigger, TriggerCommandType type);
    bool has_any_command(const ITrigger& trigger, const std::vector<TriggerCommandType>& type);
}
