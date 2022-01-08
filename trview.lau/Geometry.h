#pragma once

#include <external/DirectXTK/Inc/SimpleMath.h>

namespace trview
{
    namespace lau
    {
        DirectX::SimpleMath::Vector2 convert_uv(int32_t uv);
        int32_t make_uv(const DirectX::SimpleMath::Vector2& uv);
    }
}
