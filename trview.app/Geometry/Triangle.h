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

        DirectX::SimpleMath::Vector3 normal() const;
        DirectX::SimpleMath::Vector3 position() const;
        UniTriangle transform(const DirectX::SimpleMath::Matrix& matrix, const DirectX::SimpleMath::Color& colour_override, bool use_colour_override) const;
        mutable std::optional<DirectX::SimpleMath::Vector3> calculated_position;
    };

    // Determine whether the face should be transparent give the attribute and effects values. The 
    // mode is stored in out if it is transparent.
    // attribute: The texture attribute value.
    // effects: The face effects value.
    // Returns: True if the face is transparent. If this is false, out is not set.
    bool determine_transparency(uint16_t attribute, uint16_t effects, UniTriangle::TransparencyMode& out);
}
