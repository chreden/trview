#pragma once

#include <DirectXMath.h>

namespace trview
{
    struct ICamera
    {
        virtual ~ICamera() = 0;

        virtual DirectX::XMMATRIX view() const = 0;

        virtual DirectX::XMMATRIX projection() const = 0;

        virtual DirectX::XMMATRIX view_projection() const = 0;

        virtual DirectX::XMVECTOR position() const = 0;

        virtual DirectX::XMVECTOR up() const = 0;

        virtual DirectX::XMVECTOR forward() const = 0;
    };
}
