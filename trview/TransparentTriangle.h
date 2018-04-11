#pragma once

#include <SimpleMath.h>

namespace trview
{
    struct TransparentTriangle
    {
        enum class Mode
        {
            Normal,
            Additive
        };

        TransparentTriangle(const DirectX::SimpleMath::Vector3& v0, const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2,
            const DirectX::SimpleMath::Vector2& uv0, const DirectX::SimpleMath::Vector2& uv1, const DirectX::SimpleMath::Vector2& uv2,
            uint32_t texture, Mode mode = Mode::Normal)
            : vertices{ v0, v1, v2 }, uvs{ uv0, uv1, uv2 }, texture(texture), mode(mode)
        {
        }

        TransparentTriangle transform(const DirectX::SimpleMath::Matrix& matrix, const DirectX::SimpleMath::Color& colour) const;

        // The world space positions that make up the triangle.
        DirectX::SimpleMath::Vector3 vertices[3];
        // UV coordinates for the triangle.
        DirectX::SimpleMath::Vector2 uvs[3];
        // The world space centre position of the three vertices.
        DirectX::SimpleMath::Vector3 position;
        // The level texture index to use.
        uint32_t                     texture;
        
        Mode                         mode;

        DirectX::SimpleMath::Color   colour{ 1, 1, 1, 1 };
    };
}
