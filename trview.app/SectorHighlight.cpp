#include "SectorHighlight.h"

namespace trview
{
    void SectorHighlight::set_sector(const std::shared_ptr<Sector>& sector)
    {
        _sector = sector;
    }

    void SectorHighlight::render(graphics::Device& device, const ICamera& camera)
    {
        if (!_sector)
        {
            return;
        }
    }
}
