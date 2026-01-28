#pragma once

#include <memory>

#include "ICameraSink.h"
#include "../../Geometry/IMesh.h"

namespace trview
{
    class CameraSink final : public ICameraSink, public std::enable_shared_from_this<ICameraSink>
    {
    public:
        explicit CameraSink(const std::shared_ptr<IMesh>& mesh, const std::shared_ptr<ITextureStorage>& texture_storage,
            uint32_t number, const trlevel::tr_camera& camera, Type type, const std::vector<std::weak_ptr<IRoom>>& inferred_rooms, const std::vector<std::weak_ptr<ITrigger>>& triggers,
            const std::weak_ptr<ILevel>& level);
        virtual ~CameraSink() = default;
        virtual DirectX::BoundingBox bounding_box() const override;
        virtual uint16_t box_index() const override;
        virtual uint16_t flag() const override;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        std::weak_ptr<ILevel> level() const override;
        virtual std::vector<std::weak_ptr<IRoom>> inferred_rooms() const override;
        virtual uint32_t number() const override;
        virtual bool persistent() const override;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
        virtual DirectX::SimpleMath::Vector3 position() const override;
        void render(const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        virtual std::weak_ptr<IRoom> room() const override;
        virtual void set_type(Type type) override;
        virtual void set_visible(bool value) override;
        virtual uint16_t strength() const override;
        virtual Type type() const override;
        virtual bool visible() const override;
        virtual std::vector<std::weak_ptr<ITrigger>> triggers() const override;
        int32_t filterable_index() const override;
    private:
        uint32_t _number{ 0 };
        DirectX::SimpleMath::Vector3 _position;
        std::weak_ptr<IRoom> _room;
        uint16_t _strength{ 0u };
        uint16_t _flag{ 0 };
        std::vector<std::weak_ptr<IRoom>> _inferred_rooms;
        bool _visible{ true };
        Type _type{ Type::Camera };
        std::shared_ptr<IMesh> _mesh;
        std::vector<std::weak_ptr<ITrigger>> _triggers;
        std::weak_ptr<ILevel> _level;

        graphics::Texture _camera_texture;
        graphics::Texture _sink_texture;
    };
}
