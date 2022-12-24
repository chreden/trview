#pragma once

#include "ICameraSink.h"

namespace trview
{
    class CameraSink final : public ICameraSink
    {
    public:
        explicit CameraSink(uint32_t number, const trlevel::tr_camera& camera, uint16_t inferred_room);
        virtual ~CameraSink() = default;
        virtual uint16_t flag() const override;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        virtual uint16_t inferred_room() const override;
        virtual uint32_t number() const override;
        virtual DirectX::SimpleMath::Vector3 position() const override;
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) override;
        virtual uint16_t room() const override;
        virtual void set_type(Type type) override;
        virtual void set_visible(bool value) override;
        virtual Type type() const override;
        virtual bool visible() const override;
    private:
        uint32_t _number{ 0 };
        DirectX::SimpleMath::Vector3 _position;
        uint16_t _room{ 0 };
        uint16_t _flag{ 0 };
        uint16_t _inferred_room{ 0 };
        bool _visible{ true };
        Type _type{ Type::Camera };
    };
}
