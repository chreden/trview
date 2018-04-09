#include "stdafx.h"
#include "TransparentTriangle.h"

namespace trview
{
    TransparentTriangle TransparentTriangle::transform(const DirectX::SimpleMath::Matrix& matrix, const DirectX::SimpleMath::Color& colour) const
    {
        using namespace DirectX::SimpleMath;
        TransparentTriangle result(
            Vector3::Transform(vertices[0], matrix),
            Vector3::Transform(vertices[1], matrix),
            Vector3::Transform(vertices[2], matrix),
            uvs[0], uvs[1], uvs[2], texture);
        Vector3 minimum = Vector3::Min(Vector3::Min(result.vertices[0], result.vertices[1]), result.vertices[2]);
        Vector3 maximum = Vector3::Max(Vector3::Max(result.vertices[0], result.vertices[1]), result.vertices[2]);
        result.position = Vector3::Lerp(minimum, maximum, 0.5f);
        result.colour = colour;
        return result;
    }
}
