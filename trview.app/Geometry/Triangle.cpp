#include "Triangle.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    Vector3 UniTriangle::normal() const
    {
        return (vertices[2] - vertices[1]).Cross(vertices[1] - vertices[0]);
    }

    Vector3 UniTriangle::position() const
    {
        if (calculated_position.has_value())
        {
            return calculated_position.value();
        }
        Vector3 minimum = Vector3::Min(Vector3::Min(vertices[0], vertices[1]), vertices[2]);
        Vector3 maximum = Vector3::Max(Vector3::Max(vertices[0], vertices[1]), vertices[2]);
        calculated_position = Vector3::Lerp(minimum, maximum, 0.5f);
        return calculated_position.value();
    }

    UniTriangle UniTriangle::transform(const DirectX::SimpleMath::Matrix& matrix, const DirectX::SimpleMath::Color& colour_override, bool use_colour_override) const
    {
        UniTriangle copy = *this;
        copy.calculated_position.reset();
        copy.vertices[0] = Vector3::Transform(vertices[0], matrix);
        copy.vertices[1] = Vector3::Transform(vertices[1], matrix);
        copy.vertices[2] = Vector3::Transform(vertices[2], matrix);
        if (use_colour_override)
        {
            copy.colours[0] = colour_override;
            copy.colours[1] = colour_override;
            copy.colours[2] = colour_override;
        }
        return copy;
    }

    // Determine whether the face should be transparent give the attribute and effects values. The 
    // mode is stored in out if it is transparent.
    // attribute: The texture attribute value.
    // effects: The face effects value.
    // Returns: True if the face is transparent. If this is false, out is not set.
    bool determine_transparency(uint16_t attribute, uint16_t effects, UniTriangle::TransparencyMode& out)
    {
        // The effects value takes precendence over the attribute value.
        if (effects & 0x1)
        {
            out = UniTriangle::TransparencyMode::Additive;
            return true;
        }

        if (!attribute)
        {
            out = UniTriangle::TransparencyMode::None;
            return false;
        }

        out = attribute == 2 ? UniTriangle::TransparencyMode::Additive : UniTriangle::TransparencyMode::Normal;
        return true;
    }
}
