#pragma once

#include "ISectorHighlight.h"
#include <trview.app/Geometry/IMesh.h>

namespace trview
{
    class SectorHighlight final : public ISectorHighlight
    {
    public:
        explicit SectorHighlight(const IMesh::Source& mesh_source);
        virtual ~SectorHighlight() = default;
        virtual void set_sector(const std::shared_ptr<Sector>& sector, const DirectX::SimpleMath::Matrix& room_offset) override;
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage) override;
    private:
        DirectX::SimpleMath::Matrix _room_offset;
        std::shared_ptr<Sector> _sector;
        std::shared_ptr<IMesh> _mesh;
        IMesh::Source _mesh_source;
    };
}
