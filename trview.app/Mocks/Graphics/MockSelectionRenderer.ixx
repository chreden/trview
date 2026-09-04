module;

#include <gmock/gmock.h>
#include <SimpleMath.h>

export module trview.app:MockSelectionRenderer;

import :ISelectionRenderer;

namespace trview
{
    namespace mocks
    {
        struct MockSelectionRenderer : public ISelectionRenderer
        {
            MockSelectionRenderer();
            virtual ~MockSelectionRenderer();
            MOCK_METHOD(void, render, (const ICamera&, IRenderable&, const DirectX::SimpleMath::Color&), (override));
        };
    }
}