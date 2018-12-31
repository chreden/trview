#pragma once

#include <trview.graphics/Device.h>
#include <SimpleMath.h>

namespace trview
{
    struct ICamera;
    struct ILevelTextureStorage;
    class TransparencyBuffer;

    struct IRenderable
    {
        virtual ~IRenderable() = 0;

        virtual void render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) = 0;

        virtual void get_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) = 0;
    };
}
