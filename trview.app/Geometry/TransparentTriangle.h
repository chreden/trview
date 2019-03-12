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

        // Use the untextured texture instead of a texture from the level textures.
        const static uint32_t Untextured{ 0xffffffff };

        TransparentTriangle(const DirectX::SimpleMath::Vector3& v0, const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2,
            const DirectX::SimpleMath::Vector2& uv0, const DirectX::SimpleMath::Vector2& uv1, const DirectX::SimpleMath::Vector2& uv2,
            uint32_t texture, Mode mode, DirectX::SimpleMath::Color = { 1,1,1,1 });

        /// Create an untextured transparent triangle with the specified colour.
        /// @param v0 The first vertex.
        /// @param v1 The second vertex.
        /// @param v2 The third vertex.
        /// @param colour The colour for the triangle.
        TransparentTriangle(const DirectX::SimpleMath::Vector3& v0, const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2, const DirectX::SimpleMath::Color& colour);

        DirectX::SimpleMath::Vector3 normal() const;

        TransparentTriangle transform(const DirectX::SimpleMath::Matrix& matrix, const DirectX::SimpleMath::Color& colour_override) const;

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

    // Determine whether the face should be transparent give the attribute and effects values. The 
    // mode is stored in out if it is transparent.
    // attribute: The texture attribute value.
    // effects: The face effects value.
    // Returns: True if the face is transparent. If this is false, out is not set.
    bool determine_transparency(uint16_t attribute, uint16_t effects, TransparentTriangle::Mode& out);
}
