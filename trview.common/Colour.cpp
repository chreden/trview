#include "Colour.h"

namespace trview
{
    Colour::Colour()
        : Colour(1.0f, 1.0f, 1.0f)
    {
    }

    Colour::Colour(float r, float g, float b)
        : Colour(1.0f, r, g, b)
    {
    }

    Colour::Colour(float a, float r, float g, float b)
        : a(a), r(r), g(g), b(b)
    {
    }

    Colour::operator DirectX::SimpleMath::Color() const
    {
        return DirectX::SimpleMath::Color(r, g, b, a);
    }

    Colour& Colour::operator += (const Colour& other)
    {
        a += other.a;
        r += other.r;
        g += other.g;
        b += other.b;
        return *this;
    }

    Colour operator+(const Colour& left, const Colour& right)
    {
        return Colour(left.a + right.a, left.r + right.r, left.g + right.g, left.b + right.b);
    }

    bool operator==(const Colour& left, const Colour& right)
    {
        return left.a == right.a && left.r == right.r && left.g == right.g && left.b == right.b;
    }
}