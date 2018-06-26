#pragma once

#include <SimpleMath.h>

namespace trview
{
    struct Colour
    {
        Colour(float a, float r, float g, float b);

        operator DirectX::SimpleMath::Color() const;

        float a, r, g, b;
    };
}