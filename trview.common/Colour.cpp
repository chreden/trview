#include "Colour.h"

namespace trview
{
    Colour Colour::Black = Colour(0.0f, 0.0f, 0.0f);
    Colour Colour::Grey = Colour(0.4f, 0.4f, 0.4f);
    Colour Colour::LightGrey = Colour(0.7f, 0.7f, 0.7f);
    Colour Colour::Transparent = Colour(0.0f, 0.0f, 0.0f, 0.0f);
    Colour Colour::White = Colour(1.0f, 1.0f, 1.0f);
    Colour Colour::Green = Colour(0.0f, 1.0f, 0.0f);
    Colour Colour::Blue = Colour(0.0f, 0.0f, 1.0f);
    Colour Colour::Red = Colour(1.0f, 0.0f, 0.0f);
    Colour Colour::Yellow = Colour(1.0f, 1.0f, 0.0f);
    Colour Colour::Magenta = Colour(1.0f, 0.0f, 1.0f);
    Colour Colour::Cyan = Colour(0.0f, 1.0f, 1.0f);

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

    Colour::operator uint32_t() const
    {
        return static_cast<uint32_t>(a * 255.0f) << 24 |
               static_cast<uint32_t>(r * 255.0f) << 16 |
               static_cast<uint32_t>(g * 255.0f) << 8 |
               static_cast<uint32_t>(b * 255.0f);
    }

    Colour& Colour::operator += (const Colour& other)
    {
        a += other.a;
        r += other.r;
        g += other.g;
        b += other.b;
        return *this;
    }

    Colour named_colour(const std::wstring& name)
    {
        if (name == L"Red")
        {
            return Colour::Red;
        }
        else if (name == L"Green")
        {
            return Colour::Green;
        }
        else if (name == L"Blue")
        {
            return Colour::Blue;
        }
        else if (name == L"Magenta")
        {
            return Colour::Magenta;
        }
        else if (name == L"White")
        {
            return Colour::White;
        }
        else if (name == L"Yellow")
        {
            return Colour::Yellow;
        }
        else if (name == L"Cyan")
        {
            return Colour::Cyan;
        }

        return Colour::Black;
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