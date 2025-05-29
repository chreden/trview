#pragma once

#include "../../Geometry/IMesh.h"

namespace trview
{
    namespace mocks
    {
        struct MockMesh : public IMesh
        {
            MockMesh();
            virtual ~MockMesh();
            MOCK_METHOD(void, render, (const DirectX::SimpleMath::Matrix&, const DirectX::SimpleMath::Color&, float, DirectX::SimpleMath::Vector3, bool, bool), (override));
            MOCK_METHOD(void, render, (const DirectX::SimpleMath::Matrix&, const graphics::Texture&, const DirectX::SimpleMath::Color&, float, DirectX::SimpleMath::Vector3), (override));
            MOCK_METHOD(std::vector<TransparentTriangle>, transparent_triangles, (), (const, override));
            MOCK_METHOD(const DirectX::BoundingBox&, bounding_box, (), (const, override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
        };
    }
}