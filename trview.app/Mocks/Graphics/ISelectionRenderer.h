#pragma once

#include <trview.app/Graphics/ISelectionRenderer.h>

namespace trview
{
    class MockSelectionRenderer final : public ISelectionRenderer
    {
    public:
        MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, IRenderable&, const DirectX::SimpleMath::Color&));
    };
}