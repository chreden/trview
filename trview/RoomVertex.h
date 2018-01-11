#pragma once

#include <DirectXMath.h>

namespace trview
{
    struct room_vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 uv;
        DirectX::XMFLOAT4 colour;
    };
}