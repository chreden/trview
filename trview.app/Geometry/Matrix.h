#pragma once

#include <SimpleMath.h>
#include <trview.app/Camera/ICamera.h>

namespace trview
{
    DirectX::SimpleMath::Matrix create_billboard(
        const DirectX::SimpleMath::Vector3& position,
        const DirectX::SimpleMath::Matrix& offset,
        const DirectX::SimpleMath::Matrix& scale,
        const ICamera& camera);

    DirectX::SimpleMath::Matrix create_billboard(
        const DirectX::SimpleMath::Vector3& position,
        const DirectX::SimpleMath::Vector3& offset,
        const DirectX::SimpleMath::Matrix& scale,
        const ICamera& camera);
}
