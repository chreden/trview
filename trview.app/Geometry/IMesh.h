#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include "TransparentTriangle.h"
#include "MeshVertex.h"
#include "Triangle.h"
#include <trview.app/Graphics/ILevelTextureStorage.h>
#include <trview.app/Geometry/PickResult.h>

namespace trview
{
    struct IMesh
    {
        using Source = std::function<std::unique_ptr<IMesh>(
            const std::vector<MeshVertex>&, const std::vector<std::vector<uint32_t>>&, const std::vector<uint32_t>&,
            const std::vector<TransparentTriangle>&, const std::vector<Triangle>&)>;

        using TransparentSource = std::function<std::unique_ptr<IMesh>(const std::vector<TransparentTriangle>&, const std::vector<Triangle>&)>;

        virtual ~IMesh() = 0;

        virtual void render(const DirectX::SimpleMath::Matrix& world_view_projection,
            const ILevelTextureStorage& texture_storage,
            const DirectX::SimpleMath::Color& colour,
            DirectX::SimpleMath::Vector3 light_direction = DirectX::SimpleMath::Vector3::Zero) = 0;

        virtual const std::vector<TransparentTriangle>& transparent_triangles() const = 0;

        virtual const DirectX::BoundingBox& bounding_box() const = 0;

        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
    };
}
