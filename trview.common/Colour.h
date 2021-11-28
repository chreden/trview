#pragma once

#include <SimpleMath.h>

namespace trview
{
    struct Colour final
    {
        Colour();

        /// Create a colour from a DirectX::SimpleMath::Color.
        Colour(const DirectX::SimpleMath::Color& colour);

        /// Create a colour with the specified rgb components and a fully opaque alpha value.
        /// @param r The red value from 0 to 1.
        /// @param g The green value from 0 to 1.
        /// @param b The blue value from 0 to 1.
        Colour(float r, float g, float b);

        Colour(float a, float r, float g, float b);

        Colour(uint32_t colour);

        std::wstring code() const;

        operator DirectX::SimpleMath::Color() const;

        /// Convert the colour into a packed argb integer format.
        operator uint32_t() const;

        Colour& operator += (const Colour& other);

        float a, r, g, b;

        static Colour Black;
        static Colour Grey;
        static Colour LightGrey;
        static Colour Transparent;
        static Colour White;
        static Colour Green;
        static Colour Blue;
        static Colour Red;
        static Colour Yellow;
        static Colour Magenta;
        static Colour Cyan;
    };

    Colour from_colour_code(const std::wstring& name);
    Colour from_named_colour(const std::string& name);

    Colour operator+(const Colour& left, const Colour& right);

    bool operator==(const Colour& left, const Colour& right);
}