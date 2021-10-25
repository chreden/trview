#pragma once

#include <trview.app/Camera/ICamera.h>
#include <trview.app/Graphics/ILevelTextureStorage.h>

namespace trview
{
    struct IMover
    {
        virtual ~IMover() = 0;
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage) = 0;
        virtual void set_position(const DirectX::SimpleMath::Vector3& position) = 0;
    };
}
