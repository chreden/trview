module;

#include <SimpleMath.h>

export module trview.app:MeshVertex;

namespace trview
{
    export struct MeshVertex
    {
        DirectX::SimpleMath::Vector3 pos;
        DirectX::SimpleMath::Vector3 normal;
        DirectX::SimpleMath::Vector2 uv;
        DirectX::SimpleMath::Color colour;
    };
}
