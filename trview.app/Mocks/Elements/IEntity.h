#pragma once

#include <trview.app/Elements/IEntity.h>

namespace trview
{
    namespace mocks
    {
        struct MockEntity final : public IEntity
        {
            virtual ~MockEntity() = default;
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&));
            MOCK_METHOD(bool, visible, (), (const));
            MOCK_METHOD(void, set_visible, (bool));
            MOCK_METHOD(uint16_t, room, (), (const));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const));
            MOCK_METHOD(DirectX::BoundingBox, bounding_box, (), (const));
        };
    }
}
