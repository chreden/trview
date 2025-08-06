#pragma once

#include "TransparentTriangle.h"

namespace trview
{
    struct AnimatedTriangle
    {
        struct Frame
        {
            DirectX::SimpleMath::Vector2 uvs[3];
            uint32_t texture;
        };

        // The world space positions that make up the triangle.
        DirectX::SimpleMath::Vector3 vertices[3];
        // The world space centre position of the three vertices.
        // DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Color colours[3];
        std::vector<Frame> frames;
        uint32_t current_frame{ 0u };
        float current_time{ 0.0f };
        TransparentTriangle::Mode transparency_mode{ TransparentTriangle::Mode::None };
    };
}