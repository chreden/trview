#pragma once

#include <trview.graphics/IDevice.h>
#include <trview.app/Elements/ISector.h>
#include <trview.app/Camera/ICamera.h>

namespace trview
{
    struct ISectorHighlight
    {
        virtual ~ISectorHighlight() = 0;
        virtual void set_sector(const std::shared_ptr<ISector>& sector, const DirectX::SimpleMath::Matrix& room_offset) = 0;
        virtual void render(const ICamera& camera) = 0;
    };
}
