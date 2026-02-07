#pragma once

#include "../../Elements/CameraSink/ICameraSink.h"

namespace trview
{
    struct IRoom;

    namespace mocks
    {
        struct MockCameraSink : public ICameraSink, public std::enable_shared_from_this<MockCameraSink>
        {
            virtual ~MockCameraSink() = default;
            MOCK_METHOD(DirectX::BoundingBox, bounding_box, (), (const, override));
            MOCK_METHOD(uint16_t, box_index, (), (const, override));
            MOCK_METHOD(uint16_t, flag, (), (const, override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(std::vector<std::weak_ptr<IRoom>>, inferred_rooms, (), (const, override));
            MOCK_METHOD(std::weak_ptr<ILevel>, level, (), (const, override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(bool, persistent, (), (const, override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(void, render, (const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(std::weak_ptr<IRoom>, room, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(void, set_type, (Type), (override));
            MOCK_METHOD(uint16_t, strength, (), (const, override));
            MOCK_METHOD(Type, type, (), (const, override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<ITrigger>>, triggers, (), (const, override));
            MOCK_METHOD(int32_t, filterable_index, (), (const, override));

            bool _visible_state{ false };

            std::shared_ptr<MockCameraSink> with_number(uint32_t number)
            {
                ON_CALL(*this, number).WillByDefault(testing::Return(number));
                return shared_from_this();
            }

            std::shared_ptr<MockCameraSink> with_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
            {
                ON_CALL(*this, triggers).WillByDefault(testing::Return(triggers));
                return shared_from_this();
            }

            std::shared_ptr<MockCameraSink> with_type(ICameraSink::Type type)
            {
                ON_CALL(*this, type).WillByDefault(testing::Return(type));
                return shared_from_this();
            }

            std::shared_ptr<MockCameraSink> with_room(std::shared_ptr<IRoom> room)
            {
                ON_CALL(*this, room).WillByDefault(testing::Return(room));
                return shared_from_this();
            }

            std::shared_ptr<MockCameraSink> with_updating_visible(bool value)
            {
                _visible_state = value;
                ON_CALL(*this, visible).WillByDefault([&]() { return _visible_state; });
                ON_CALL(*this, set_visible).WillByDefault([&](auto v) { _visible_state = v; });
                return shared_from_this();
            }
        };
    }
}
