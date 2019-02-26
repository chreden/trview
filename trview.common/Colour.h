#pragma once

#include <SimpleMath.h>

namespace trview
{
    struct Colour final
    {
        Colour();

        /// Create a colour with the specified rgb components and a fully opaque alpha value.
        /// @param r The red value from 0 to 1.
        /// @param g The green value from 0 to 1.
        /// @param b The blue value from 0 to 1.
        Colour(float r, float g, float b);

        Colour(float a, float r, float g, float b);

        operator DirectX::SimpleMath::Color() const;

        Colour& operator += (const Colour& other);

        float a, r, g, b;

        static Colour Gray;
        static Colour Transparent;
        static Colour White;
    };

    Colour operator+(const Colour& left, const Colour& right);

    bool operator==(const Colour& left, const Colour& right);
}