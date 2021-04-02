#pragma once

#include <cstdint>
#include <trview.graphics/Device.h>
#include <trview.app/Geometry/Mesh.h>
#include <trview.app/Elements/Sector.h>
#include <trview.app/Camera/ICamera.h>
#include "ILevelTextureStorage.h"

namespace trview
{
    class SectorHighlight final
    {
    public:
        void set_sector(const std::shared_ptr<Sector>& sector, const DirectX::SimpleMath::Matrix& room_offset);
        void render(graphics::IDevice& device, const ICamera& camera, const ILevelTextureStorage& texture_storage);
    private:
        DirectX::SimpleMath::Matrix _room_offset;
        std::shared_ptr<Sector> _sector;
        std::unique_ptr<Mesh> _mesh;
    };
}
