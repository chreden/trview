#pragma once

#include <cstdint>
#include <external/DirectXTK/Inc/SimpleMath.h>
#include <trview.app/Geometry/IRenderable.h>
#include <trview.app/Geometry/PickResult.h>
#include <trlevel/tr_lights.h>
#include "../Filters/IFilterable.h"

namespace trview
{
    struct IRoom;

    struct ILight : public IRenderable, public IFilterable
    {
        Event<> on_changed;

        using Source = std::function<std::shared_ptr<ILight>(uint32_t, const std::weak_ptr<IRoom>&, const trlevel::tr_x_room_light&, const std::weak_ptr<ILevel>&)>;
        virtual ~ILight() = 0;
        virtual uint32_t number() const = 0;
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        virtual Colour colour() const = 0;
        virtual std::weak_ptr<IRoom> room() const = 0;
        virtual trlevel::LightType type() const = 0;
        virtual int32_t intensity() const = 0;
        virtual int32_t fade() const = 0;
        virtual DirectX::SimpleMath::Vector3 direction() const = 0;
        virtual std::weak_ptr<ILevel> level() const = 0;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
        virtual float in() const = 0;
        virtual float out() const = 0;
        virtual float rad_in() const = 0;
        virtual float rad_out() const = 0;
        virtual float range() const = 0;
        virtual float length() const = 0;
        virtual float cutoff() const = 0;
        virtual float radius() const = 0;
        virtual float density() const = 0;
        virtual void set_position(const DirectX::SimpleMath::Vector3& position) = 0;
        virtual void render_direction(const ICamera& camera) = 0;
        virtual trlevel::LevelVersion level_version() const = 0;
    };

    bool has_colour(const ILight& light);
    bool has_direction(const ILight& light);
    bool has_fade(const ILight& light);
    bool has_intensity(const ILight& light);
    bool has_position(const ILight& light);
    bool has_hotspot(const ILight& light);
    bool has_falloff(const ILight& light);
    bool has_falloff_angle(const ILight& light);
    bool has_length(const ILight& light);
    bool has_cutoff(const ILight& light);
    bool has_rad_in(const ILight& light);
    bool has_rad_out(const ILight& light);
    bool has_range(const ILight& light);
    bool has_density(const ILight& light);
    bool has_radius(const ILight& light);
    float fade(const ILight& light);
    float intensity(const ILight& light);
    float hotspot(const ILight& light);
    float falloff(const ILight& light);
    float falloff_angle(const ILight& light);
    float length(const ILight& light);
    float cutoff(const ILight& light);
    float rad_in(const ILight& light);
    float rad_out(const ILight& light);
    float range(const ILight& light);
    float density(const ILight& light);
    float radius(const ILight& light);

    uint32_t light_room(const std::shared_ptr<ILight>& light);
    uint32_t light_room(const ILight& light);
}
