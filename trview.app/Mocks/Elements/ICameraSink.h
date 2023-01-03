#pragma once

#include "../../Elements/CameraSink/ICameraSink.h"

namespace trview
{
    namespace mocks
    {
        struct MockCameraSink : public ICameraSink, public std::enable_shared_from_this<MockCameraSink>
        {
            virtual ~MockCameraSink() = default;
            MOCK_METHOD(DirectX::BoundingBox, bounding_box, (), (const, override));
            MOCK_METHOD(uint16_t, box_index, (), (const, override));
            MOCK_METHOD(uint16_t, flag, (), (const, override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(std::vector<uint16_t>, inferred_rooms, (), (const, override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(bool, persistent, (), (const, override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(uint16_t, room, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(void, set_type, (Type), (override));
            MOCK_METHOD(uint16_t, strength, (), (const, override));
            MOCK_METHOD(Type, type, (), (const, override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<ITrigger>>, triggers, (), (const, override));

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

            std::shared_ptr<MockCameraSink> with_room(uint16_t room)
            {
                ON_CALL(*this, room).WillByDefault(testing::Return(room));
                return shared_from_this();
            }
        };
    }
}
