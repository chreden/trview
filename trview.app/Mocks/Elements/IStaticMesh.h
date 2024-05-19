#pragma once

#include <trview.app/Elements/IStaticMesh.h>

namespace trview
{
    namespace mocks
    {
        struct MockStaticMesh : public IStaticMesh
        {
            MockStaticMesh();
            virtual ~MockStaticMesh();
            MOCK_METHOD(DirectX::BoundingBox, collision, (), (const, override));
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, render_bounding_box, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(std::weak_ptr<IRoom>, room, (), (const, override));
            MOCK_METHOD(float, rotation, (), (const, override));
            MOCK_METHOD(DirectX::BoundingBox, visibility, (), (const, override));
            MOCK_METHOD(Type, type, (), (const, override));
            MOCK_METHOD(uint16_t, id, (), (const, override));
            MOCK_METHOD(void, set_number, (uint32_t), (override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
        };
    }
}
