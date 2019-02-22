#pragma once

#include <cstdint>
#include <trview.graphics/Device.h>
#include "Sector.h"
#include "ICamera.h"

namespace trview
{
    class SectorHighlight final
    {
    public:
        void set_sector(const std::shared_ptr<Sector>& sector);
        void render(graphics::Device& device, const ICamera& camera);
    private:
        bool _visible{ false };
        std::shared_ptr<Sector> _sector;
    };
}
