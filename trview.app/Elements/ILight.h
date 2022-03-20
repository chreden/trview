#pragma once

#include <cstdint>
#include <external/DirectXTK/Inc/SimpleMath.h>
#include <trview.app/Geometry/IRenderable.h>
#include <trview.app/Geometry/PickResult.h>
#include <trlevel/tr_lights.h>

namespace trview
{
    struct ILight : public IRenderable
    {
        using Source = std::function<std::shared_ptr<ILight>(uint32_t, uint32_t, const trlevel::tr_x_room_light&)>;
        virtual ~ILight() = 0;
        virtual uint32_t number() const = 0;
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        virtual Colour colour() const = 0;
        virtual uint32_t room() const = 0;
        virtual trlevel::LightType type() const = 0;
        virtual int32_t intensity() const = 0;
        virtual int32_t fade() const = 0;
        virtual DirectX::SimpleMath::Vector3 direction() const = 0;
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
        virtual void render_direction(const ICamera& camera, const ILevelTextureStorage& texture_storage) = 0;
        virtual trlevel::LevelVersion level_version() const = 0;
    };

    bool has_colour(const ILight& light);
    bool has_direction(const ILight& light);
    bool has_fade(const ILight& light);
    bool has_intensity(const ILight& light);
    bool has_position(const ILight& light);
}
