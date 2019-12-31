#pragma once

#include <cstdint>
#include <vector>
#include <trview.app/Geometry/TransparentTriangle.h>
#include <trview.app/Geometry/PickResult.h>
#include <trview.app/Geometry/Mesh.h>
#include <trview.app/Geometry/IRenderable.h>

namespace trview
{
    struct TriggerInfo;

    enum class TriggerType
    {
        Trigger, Pad, Switch, Key, Pickup, HeavyTrigger, Antipad, Combat, Dummy,
        AntiTrigger, HeavySwitch, HeavyAntiTrigger, Monkey, Skeleton, Tightrope, Crawl, Climb
    };

    enum class TriggerCommandType
    {
        Object, Camera, UnderwaterCurrent, FlipMap, FlipOn, FlipOff, LookAtItem,
        EndLevel, PlaySoundtrack, Flipeffect, SecretFound, ClearBodies, Flyby, Cutscene
    };

    class Command final
    {
    public:
        Command(uint32_t number, TriggerCommandType type, uint16_t index);
        uint32_t number() const;
        TriggerCommandType type() const;
        uint16_t index() const;
    private:
        uint32_t _number;
        TriggerCommandType _type;
        uint16_t _index;
    };

    class TransparencyBuffer;
    struct ICamera;

    class Trigger final : public IRenderable
    {
    public:
        explicit Trigger(uint32_t number, uint32_t room, uint16_t x, uint16_t z, const TriggerInfo& trigger_info);
        virtual ~Trigger() = default;

        uint32_t    number() const;
        uint32_t    room() const;
        uint16_t    x() const;
        uint16_t    z() const;
        bool        triggers_item(uint32_t index) const;
        TriggerType type() const;
        bool        only_once() const;
        uint16_t    flags() const;
        uint8_t     timer() const;
        uint16_t    sector_id() const;
        const std::vector<Command>& commands() const;
        const std::vector<TransparentTriangle>& triangles() const;
        void set_triangles(const std::vector<TransparentTriangle>& transparent_triangles);
        PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const;
        bool has_command(TriggerCommandType type) const;
        void set_position(const DirectX::SimpleMath::Vector3& position);
        DirectX::SimpleMath::Vector3 position() const;

        virtual void render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) override;
        virtual void get_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
    private:
        std::vector<uint16_t> _objects;
        std::vector<Command> _commands;
        std::unique_ptr<Mesh> _mesh;
        DirectX::SimpleMath::Vector3 _position;
        TriggerType _type;
        uint32_t _number;
        uint32_t _room;
        uint16_t _x;
        uint16_t _z;
        bool _only_once;
        uint16_t _flags;
        uint8_t _timer;
        uint16_t _sector_id;
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
}