module;

#include <gmock/gmock.h>
#include <SimpleMath.h>

export module trview.app.mocks:MockSelectionRenderer;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockSelectionRenderer : public ISelectionRenderer
        {
            MOCK_METHOD(void, render, (const ICamera&, IRenderable&, const DirectX::SimpleMath::Color&), (override));
        };
    }
}