#pragma once

#include <trview.app/Elements/IStaticMesh.h>

namespace trview
{
    namespace mocks
    {
        struct MockStaticMesh final : public IStaticMesh
        {
            virtual ~MockStaticMesh() = default;
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, render_bounding_box, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
        };
    }
}
