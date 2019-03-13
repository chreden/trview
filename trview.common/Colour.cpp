#include "Colour.h"

namespace trview
{
    Colour Colour::Black = Colour(0.0f, 0.0f, 0.0f);
    Colour Colour::Grey = Colour(0.4f, 0.4f, 0.4f);
    Colour Colour::LightGrey = Colour(0.7f, 0.7f, 0.7f);
    Colour Colour::Transparent = Colour(0.0f, 0.0f, 0.0f, 0.0f);
    Colour Colour::White = Colour(1.0f, 1.0f, 1.0f);

    Colour::Colour()
        : Colour(1.0f, 1.0f, 1.0f)
    {
    }

    Colour::Colour(const DirectX::SimpleMath::Color& color)
        : Colour(color.A(), color.R(), color.G(), color.B())
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