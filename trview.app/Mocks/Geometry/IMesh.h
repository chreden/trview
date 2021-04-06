#pragma once

#include <trview.app/Geometry/IMesh.h>

namespace trview
{
    namespace mocks
    {
        class MockMesh : public IMesh
        {
        public:
            virtual ~MockMesh() = default;
            MOCK_METHOD(void, render, (const DirectX::SimpleMath::Matrix&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&, DirectX::SimpleMath::Vector3));
            MOCK_METHOD(const std::vector<TransparentTriangle>&, transparent_triangles, (), (const));
            MOCK_METHOD(const DirectX::BoundingBox&, bounding_box, (), (const));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const));
        };
    }
}