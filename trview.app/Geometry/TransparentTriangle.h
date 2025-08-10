#pragma once

#include <SimpleMath.h>

namespace trview
{
    /// <summary>
    /// The triangle that does it all.
    /// </summary>
    struct UniTriangle
    {
        enum class AnimationMode
        {
            None,
            Swap,
            UV
        };

        enum class TransparencyMode
        {
            None,
            Normal,
            Additive
        };

        enum class CollisionMode
        {
            Disabled,
            Enabled
        };

        enum class TextureMode
        {
            Textured,
            Untextured
        };

        enum class SideMode
        {
            Single,
            Double
        };

        struct Frame
        {
            DirectX::SimpleMath::Vector2 uvs[3];
            uint32_t texture;
        };

        AnimationMode animation_mode{ AnimationMode::None };
        CollisionMode collision_mode{ CollisionMode::Enabled };
        DirectX::SimpleMath::Color colours[3];
        uint32_t current_frame{ 0u };
        float current_time{ 0.0f };
        std::vector<Frame> frames;
        float frame_time{ 0.0f };
        DirectX::SimpleMath::Vector3 normals[3];
        SideMode side_mode{ SideMode::Single };
        TextureMode texture_mode{ TextureMode::Textured };
        TransparencyMode transparency_mode{ TransparencyMode::None };
        DirectX::SimpleMath::Vector3 vertices[3];

        DirectX::SimpleMath::Vector3 normal() const
        {
            return (vertices[2] - vertices[1]).Cross(vertices[1] - vertices[0]);
        }
    };

    struct TransparentTriangle
    {
        // Use the untextured texture instead of a texture from the level textures.
        const static uint32_t Untextured{ 0xffffffff };

        TransparentTriangle(const DirectX::SimpleMath::Vector3& v0, const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2,
            const DirectX::SimpleMath::Vector2& uv0, const DirectX::SimpleMath::Vector2& uv1, const DirectX::SimpleMath::Vector2& uv2,
            uint32_t texture, UniTriangle::TransparencyMode mode, const DirectX::SimpleMath::Color& c0, const DirectX::SimpleMath::Color& c1, const DirectX::SimpleMath::Color& c2);

        /// Create an untextured transparent triangle with the specified colour.
        /// @param v0 The first vertex.
        /// @param v1 The second vertex.
        /// @param v2 The third vertex.
        /// @param colour The colour for the triangle.
        TransparentTriangle(const DirectX::SimpleMath::Vector3& v0, const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2, const DirectX::SimpleMath::Color& c0, const DirectX::SimpleMath::Color& c1, const DirectX::SimpleMath::Color& c2);

        DirectX::SimpleMath::Vector3 normal() const;

        TransparentTriangle transform(const DirectX::SimpleMath::Matrix& matrix, const DirectX::SimpleMath::Color& colour_override, bool use_colour_override) const;

        // The world space positions that make up the triangle.
        DirectX::SimpleMath::Vector3 vertices[3];
        // UV coordinates for the triangle.
        DirectX::SimpleMath::Vector2 uvs[3];
        // The world space centre position of the three vertices.
        DirectX::SimpleMath::Vector3 position;
        // The level texture index to use.
        uint32_t                     texture;
        
        UniTriangle::TransparencyMode                         mode;

        DirectX::SimpleMath::Color   colours[3];
    };

    // Determine whether the face should be transparent give the attribute and effects values. The 
    // mode is stored in out if it is transparent.
    // attribute: The texture attribute value.
    // effects: The face effects value.
    // Returns: True if the face is transparent. If this is false, out is not set.
    bool determine_transparency(uint16_t attribute, uint16_t effects, UniTriangle::TransparencyMode& out);
}
