#pragma once

#include "../../Graphics/ISelectionRenderer.h"

namespace trview
{
    namespace mocks
    {
        struct MockSelectionRenderer : public ISelectionRenderer
        {
            MockSelectionRenderer();
            virtual ~MockSelectionRenderer();
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, IRenderable&, const DirectX::SimpleMath::Color&), (override));
        };
    }
}