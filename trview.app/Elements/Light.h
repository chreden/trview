#pragma once

#include <memory>

#include "ILight.h"
#include <trview.app/Geometry/IMesh.h>

namespace trview
{
    class Light final : public ILight, public std::enable_shared_from_this<ILight>
    {
    public:
        explicit Light(const std::shared_ptr<IMesh>& mesh, uint32_t number, const std::weak_ptr<IRoom>& room, const trlevel::tr_x_room_light& light, const std::weak_ptr<ILevel>& level);
        virtual ~Light() = default;
        virtual uint32_t number() const override;
        virtual DirectX::SimpleMath::Vector3 position() const override;
        virtual Colour colour() const override;
        std::weak_ptr<IRoom> room() const override;
        virtual trlevel::LightType type() const override;
        virtual int32_t intensity() const override;
        virtual int32_t fade() const override;
        virtual DirectX::SimpleMath::Vector3 direction() const override;
        std::weak_ptr<ILevel> level() const override;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
        void render(const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        void render_direction(const ICamera& camera) override;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        virtual bool visible() const override;
        virtual void set_visible(bool value) override;
        virtual float in() const override;
        virtual float out() const override;
        virtual float rad_in() const override;
        virtual float rad_out() const override;
        virtual float range() const override;
        virtual float length() const override;
        virtual float cutoff() const override;
        virtual float radius() const override;
        virtual float density() const override;
        virtual void set_position(const DirectX::SimpleMath::Vector3& position) override;
        virtual trlevel::LevelVersion level_version() const override;
        int32_t filterable_index() const override;
    private:
        std::shared_ptr<IMesh> _mesh;
        std::weak_ptr<ILevel> _level;

        uint32_t _number;
        std::weak_ptr<IRoom> _room;
        bool _visible{ true };
        DirectX::SimpleMath::Vector3 _position;
        Colour _colour;
        trlevel::LightType _type;
        int32_t _intensity{ 0u };
        int32_t _fade{ 0u };
        DirectX::SimpleMath::Vector3 _direction;
        float _in{ 0 };
        float _out{ 0 };
        float _rad_in{ 0 };
        float _rad_out{ 0 };
        float _range{ 0 };
        float _length{ 0 };
        float _cutoff{ 0 };
        float _radius{ 0 };
        float _density{ 0 };
        trlevel::PlatformAndVersion _version;
    };
}
