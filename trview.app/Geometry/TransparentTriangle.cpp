#include "TransparentTriangle.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    TransparentTriangle::TransparentTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector2& uv0, const Vector2& uv1, const Vector2& uv2, uint32_t texture, Mode mode, const DirectX::SimpleMath::Color& c0, const DirectX::SimpleMath::Color& c1, const DirectX::SimpleMath::Color& c2)
        : vertices{ v0, v1, v2 }, uvs{ uv0, uv1, uv2 }, texture(texture), mode(mode), colours{ c0, c1, c2 }
    {
        Vector3 minimum = Vector3::Min(Vector3::Min(v0, v1), v2);
        Vector3 maximum = Vector3::Max(Vector3::Max(v0, v1), v2);
        position = Vector3::Lerp(minimum, maximum, 0.5f);
    }

    TransparentTriangle::TransparentTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const DirectX::SimpleMath::Color& c0, const DirectX::SimpleMath::Color& c1, const DirectX::SimpleMath::Color& c2)
        : TransparentTriangle(v0, v1, v2, Vector2::Zero, Vector2::Zero, Vector2::Zero, Untextured, Mode::Normal, c0, c1, c2)
    {
    }

    Vector3 TransparentTriangle::normal() const
    {
        auto first = vertices[1] - vertices[0];
        auto second = vertices[2] - vertices[0];
        first.Normalize();
        second.Normalize();
        return first.Cross(second);
    }

    TransparentTriangle TransparentTriangle::transform(const DirectX::SimpleMath::Matrix& matrix, const DirectX::SimpleMath::Color& colour_override, bool use_colour_override) const
    {
        using namespace DirectX::SimpleMath;
        return TransparentTriangle(
            Vector3::Transform(vertices[0], matrix),
            Vector3::Transform(vertices[1], matrix),
            Vector3::Transform(vertices[2], matrix),
            uvs[0], uvs[1], uvs[2], texture, mode, 
            use_colour_override ? colour_override : colours[0],
            use_colour_override ? colour_override : colours[1],
            use_colour_override ? colour_override : colours[2]);
    }

    // Determine whether the face should be transparent give the attribute and effects values. The 
    // mode is stored in out if it is transparent.
    // attribute: The texture attribute value.
    // effects: The face effects value.
    // Returns: True if the face is transparent. If this is false, out is not set.
    bool determine_transparency(uint16_t attribute, uint16_t effects, TransparentTriangle::Mode& out)
    {
        // The effects value takes precendence over the attribute value.
        if (effects & 0x1)
        {
            out = TransparentTriangle::Mode::Additive;
            return true;
        }

        if (!attribute)
        {
            return false;
        }

        out = attribute == 2 ? TransparentTriangle::Mode::Additive : TransparentTriangle::Mode::Normal;
        return true;
    }
}
