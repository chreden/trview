#pragma once

namespace trview
{
    struct Triangle
    {
        Triangle(const DirectX::SimpleMath::Vector3& v0, const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2)
            : v0(v0), v1(v1), v2(v2), normal((v2 - v0).Cross(v1 - v0))
        {
        }

        DirectX::SimpleMath::Vector3 v0;
        DirectX::SimpleMath::Vector3 v1;
        DirectX::SimpleMath::Vector3 v2;
        DirectX::SimpleMath::Vector3 normal;
    };
}
