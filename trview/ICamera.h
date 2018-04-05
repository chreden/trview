#pragma once

#include <SimpleMath.h>

namespace trview
{
    struct ICamera
    {
        virtual ~ICamera() = 0;

        virtual DirectX::SimpleMath::Matrix view() const = 0;

        virtual DirectX::SimpleMath::Matrix projection() const = 0;

        virtual DirectX::SimpleMath::Matrix view_projection() const = 0;

        virtual DirectX::SimpleMath::Vector3 position() const = 0;

        virtual DirectX::SimpleMath::Vector3 up() const = 0;

        virtual DirectX::SimpleMath::Vector3 forward() const = 0;
    };
}
