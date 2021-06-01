#pragma once

#include <cstdint>
#include <SimpleMath.h>
#include <trlevel/trtypes.h>
#include <trview.app/Camera/ICamera.h>
#include <trview.app/Geometry/IMesh.h>

namespace trview
{
    struct ILevelTextureStorage;
    struct ITransparencyBuffer;

    struct IStaticMesh
    {
        using PositionSource = std::function<std::shared_ptr<IStaticMesh>(const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Matrix&, std::shared_ptr<IMesh>)>;
        using MeshSource = std::function<std::shared_ptr<IStaticMesh>(const trlevel::tr3_room_staticmesh&, const trlevel::tr_staticmesh&, const std::shared_ptr<IMesh>&)>;

        virtual ~IStaticMesh() = 0;
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) = 0;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) = 0;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
    };
}
