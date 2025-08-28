#pragma once

#include <vector>
#include "IPortal.h"

namespace trview
{
    class Portal final : public IPortal
    {
    public:
        virtual ~Portal() = default;
        explicit Portal(const trlevel::tr_room_portal& portal, DirectX::SimpleMath::Vector3 room_offset);
        void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera) override;
        DirectX::SimpleMath::Vector3 normal() const override;
        uint16_t room() const override;
        std::vector<DirectX::SimpleMath::Vector3> vertices() const override;
    private:
        std::vector<DirectX::SimpleMath::Vector3> _vertices;
        DirectX::SimpleMath::Vector3 _room_offset;
        DirectX::SimpleMath::Vector3 _normal;
        uint16_t _room{ 0u };
    };
}
