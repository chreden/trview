#pragma once

#include <cstdint>
#include <trview.graphics/Device.h>
#include "Sector.h"
#include "ICamera.h"
#include "ILevelTextureStorage.h"

namespace trview
{
    class SectorHighlight final
    {
    public:
        void set_sector(const std::shared_ptr<Sector>& sector, const DirectX::SimpleMath::Matrix& room_offset);
        void render(graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage);
    private:
        DirectX::SimpleMath::Matrix _room_offset;
        std::shared_ptr<Sector> _sector;
    };
}
