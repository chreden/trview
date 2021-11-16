#include "Colour.h"
#include <trview.common/Json.h>

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

    Colour::Colour(uint32_t colour)
        : Colour((colour >> 16 & 0xFF) / 255.0f, (colour >> 8 & 0xFF) / 255.0f, (colour & 0xFF) / 255.0f)
    {
    }

    std::wstring Colour::name() const
    {
        uint32_t value = *this;
        return std::to_wstring(value);
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
        uint32_t value = std::stoul(name);
        return Colour(
            ((value >> 24) & 0xff) / 255.0f,
            ((value >> 16) & 0xff) / 255.0f,
            ((value >> 8) & 0xff) / 255.0f,
            (value & 0xff) / 255.0f);
    }

    Colour operator+(const Colour& left, const Colour& right)
    {
        return Colour(left.a + right.a, left.r + right.r, left.g + right.g, left.b + right.b);
    }

    bool operator==(const Colour& left, const Colour& right)
    {
        return left.a == right.a && left.r == right.r && left.g == right.g && left.b == right.b;
    }

    void from_json(const nlohmann::json& json, Colour& colour)
    {
        if (json.is_string())
        {
            const auto str = json.get<std::string>();
            if (str == "transparent")
            {
                colour = Colour::Transparent;
            }
            else
            {
                colour = Colour::White;
            }
        }
        else
        {
            colour = Colour(read_attribute<float>(json, "a", 1.0f),
                read_attribute<float>(json, "r", 0.0f),
                read_attribute<float>(json, "g", 0.0f),
                read_attribute<float>(json, "b", 0.0f));
        }
    }
}