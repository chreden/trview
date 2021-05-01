#pragma once

#include <trview.app/Geometry/IMesh.h>
#include <trview.app/Elements/ITrigger.h>

namespace trview
{
    struct TriggerInfo;

    class TransparencyBuffer;
    struct ICamera;

    class Trigger final : public ITrigger
    {
    public:
        explicit Trigger(uint32_t number, uint32_t room, uint16_t x, uint16_t z, const TriggerInfo& trigger_info, const IMesh::TransparentSource& mesh_source);
        virtual ~Trigger() = default;

        virtual uint32_t number() const override;
        virtual uint32_t room() const override;
        virtual uint16_t x() const override;
        virtual uint16_t z() const override;
        virtual bool triggers_item(uint32_t index) const override;
        virtual TriggerType type() const override;
        virtual bool only_once() const override;
        virtual uint16_t flags() const override;
        virtual uint8_t timer() const override;
        virtual uint16_t sector_id() const override;
        virtual const std::vector<Command>& commands() const override;
        virtual const std::vector<TransparentTriangle>& triangles() const override;
        virtual void set_triangles(const std::vector<TransparentTriangle>& transparent_triangles) override;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
        virtual bool has_command(TriggerCommandType type) const override;
        virtual bool has_any_command(const std::vector<TriggerCommandType>& type) const override;
        virtual void set_position(const DirectX::SimpleMath::Vector3& position) override;
        virtual DirectX::SimpleMath::Vector3 position() const override;

        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) override;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        virtual bool visible() const override;
        virtual void set_visible(bool value) override;
    private:
        std::vector<uint16_t> _objects;
        std::vector<Command> _commands;
        std::unique_ptr<IMesh> _mesh;
        DirectX::SimpleMath::Vector3 _position;
        IMesh::TransparentSource _mesh_source;
        TriggerType _type;
        uint32_t _number;
        uint32_t _room;
        uint16_t _x;
        uint16_t _z;
        bool _only_once;
        uint16_t _flags;
        uint8_t _timer;
        uint16_t _sector_id;
        bool _visible{ true };
    };
}