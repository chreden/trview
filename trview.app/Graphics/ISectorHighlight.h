#pragma once

#include <trview.graphics/IDevice.h>
#include <trview.app/Elements/Sector.h>
#include <trview.app/Camera/ICamera.h>
#include "ILevelTextureStorage.h"

namespace trview
{
    struct ISectorHighlight
    {
        virtual ~ISectorHighlight() = 0;
        virtual void set_sector(const std::shared_ptr<Sector>& sector, const DirectX::SimpleMath::Matrix& room_offset) = 0;
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage) = 0;
    };
}
