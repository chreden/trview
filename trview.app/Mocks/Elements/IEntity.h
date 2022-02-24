#pragma once

#include <trview.app/Elements/IEntity.h>

namespace trview
{
    namespace mocks
    {
        struct MockEntity : public IEntity
        {
            virtual ~MockEntity() = default;
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(uint16_t, room, (), (const, override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(DirectX::BoundingBox, bounding_box, (), (const, override));
            MOCK_METHOD(void, adjust_y, (float), (override));
            MOCK_METHOD(bool, needs_ocb_adjustment, (), (const, override));
        };
    }
}
