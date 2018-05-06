#include "stdafx.h"
#include "TransparentTriangle.h"

namespace trview
{
    TransparentTriangle TransparentTriangle::transform(const DirectX::SimpleMath::Matrix& matrix, const DirectX::SimpleMath::Color& colour_override) const
    {
        using namespace DirectX::SimpleMath;
        TransparentTriangle result(
            Vector3::Transform(vertices[0], matrix),
            Vector3::Transform(vertices[1], matrix),
            Vector3::Transform(vertices[2], matrix),
            uvs[0], uvs[1], uvs[2], texture, mode);
        Vector3 minimum = Vector3::Min(Vector3::Min(result.vertices[0], result.vertices[1]), result.vertices[2]);
        Vector3 maximum = Vector3::Max(Vector3::Max(result.vertices[0], result.vertices[1]), result.vertices[2]);
        result.position = Vector3::Lerp(minimum, maximum, 0.5f);
        result.colour = colour_override;
        return result;
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
