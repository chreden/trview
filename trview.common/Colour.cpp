#include "Colour.h"

namespace trview
{
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
}