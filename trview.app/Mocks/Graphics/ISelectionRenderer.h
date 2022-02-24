#pragma once

#include "../../Graphics/ISelectionRenderer.h"

namespace trview
{
    namespace mocks
    {
        struct MockSelectionRenderer : public ISelectionRenderer
        {
            virtual ~MockSelectionRenderer() = default;
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, IRenderable&, const DirectX::SimpleMath::Color&), (override));
        };
    }
}