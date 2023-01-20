#pragma once

#include "../../Elements/IItem.h"

namespace trview
{
    namespace mocks
    {
        struct MockItem : public IItem
        {
            MockItem();
            virtual ~MockItem();
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(uint16_t, room, (), (const, override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(DirectX::BoundingBox, bounding_box, (), (const, override));
            MOCK_METHOD(void, adjust_y, (float), (override));
            MOCK_METHOD(bool, needs_ocb_adjustment, (), (const, override));
            MOCK_METHOD(std::string, type, (), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<ITrigger>>, triggers, (), (const, override));
            MOCK_METHOD(uint32_t, type_id, (), (const, override));
            MOCK_METHOD(int32_t, ocb, (), (const, override));
            MOCK_METHOD(uint16_t, activation_flags, (), (const, override));
            MOCK_METHOD(bool, clear_body_flag, (), (const, override));
            MOCK_METHOD(bool, invisible_flag, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
        };
    }
}
