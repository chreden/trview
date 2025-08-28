#pragma once

#include <functional>
#include <memory>

#include <trlevel/trtypes.h>

namespace trview
{
    struct ITransparencyBuffer;
    struct ICamera;

    struct IPortal
    {
        using Source = std::function<std::shared_ptr<IPortal>(const trlevel::tr_room_portal&, const DirectX::SimpleMath::Vector3&)>;
        virtual ~IPortal() = 0;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera) = 0;
        virtual DirectX::SimpleMath::Vector3 normal() const = 0;
        virtual uint16_t room() const = 0;
        virtual std::vector<DirectX::SimpleMath::Vector3> vertices() const = 0;
    };
}
