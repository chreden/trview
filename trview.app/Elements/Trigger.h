#pragma once

#include <memory>

#include <trview.app/Geometry/IMesh.h>
#include <trview.app/Elements/ITrigger.h>
#include <trview.app/Camera/ICamera.h>
#include "../Elements/ISector.h"

namespace trview
{
    struct TriggerInfo;

    class Trigger final : public ITrigger, public std::enable_shared_from_this<ITrigger>
    {
    public:
        explicit Trigger(uint32_t number, const std::weak_ptr<IRoom>& room, uint16_t x, uint16_t z, const TriggerInfo& trigger_info, trlevel::LevelVersion level_version, const std::weak_ptr<ILevel>& level, const std::weak_ptr<ISector>& sector, const IMesh::Source& mesh_source);
        virtual ~Trigger() = default;
        virtual uint32_t number() const override;
        std::weak_ptr<IRoom> room() const override;
        virtual uint16_t x() const override;
        virtual uint16_t z() const override;
        virtual bool triggers_item(uint32_t index) const override;
        virtual TriggerType type() const override;
        virtual bool only_once() const override;
        virtual uint16_t flags() const override;
        virtual int16_t timer() const override;
        virtual uint16_t sector_id() const override;
        Colour colour() const override;
        std::vector<Command> commands() const override;
        void set_colour(const std::optional<Colour>& colour) override;
        void set_triangles(const std::vector<Triangle>& transparent_triangles) override;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
        virtual void set_position(const DirectX::SimpleMath::Vector3& position) override;
        virtual DirectX::SimpleMath::Vector3 position() const override;
        void render(const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        virtual bool visible() const override;
        virtual void set_visible(bool value) override;
        std::weak_ptr<ILevel> level() const override;
        std::weak_ptr<ISector> sector() const override;
        int32_t filterable_index() const override;
    private:
        std::vector<uint16_t> _objects;
        std::vector<Command> _commands;
        std::shared_ptr<IMesh> _mesh;
        DirectX::SimpleMath::Vector3 _position;
        IMesh::Source _mesh_source;
        TriggerType _type;
        uint32_t _number;
        std::weak_ptr<IRoom> _room;
        uint16_t _x;
        uint16_t _z;
        bool _only_once;
        uint16_t _flags;
        int16_t _timer;
        uint16_t _sector_id;
        bool _visible{ true };
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Unknown };
        std::weak_ptr<ILevel> _level;
        std::optional<Colour> _colour;
        std::weak_ptr<ISector> _sector;
    };
}