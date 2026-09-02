module;

#include <SimpleMath.h>

export module trview.app:Matrix;

import :ICamera;

namespace trview
{
    export DirectX::SimpleMath::Matrix create_billboard(
        const DirectX::SimpleMath::Vector3& position,
        const DirectX::SimpleMath::Matrix& offset,
        const DirectX::SimpleMath::Matrix& scale,
        const ICamera& camera);

    export DirectX::SimpleMath::Matrix create_billboard(
        const DirectX::SimpleMath::Vector3& position,
        const DirectX::SimpleMath::Vector3& offset,
        const DirectX::SimpleMath::Matrix& scale,
        const ICamera& camera);
}
