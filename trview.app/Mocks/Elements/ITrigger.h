#pragma once

#include "../../Elements/ITrigger.h"

namespace trview
{
    namespace mocks
    {
        struct MockTrigger : public ITrigger, public std::enable_shared_from_this<MockTrigger>
        {
            virtual ~MockTrigger() = default;
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(uint32_t, room, (), (const, override));
            MOCK_METHOD(uint16_t, x, (), (const, override));
            MOCK_METHOD(uint16_t, z, (), (const, override));
            MOCK_METHOD(bool, triggers_item, (uint32_t index), (const, override));
            MOCK_METHOD(TriggerType, type, (), (const, override));
            MOCK_METHOD(bool, only_once, (), (const, override));
            MOCK_METHOD(uint16_t, flags, (), (const, override));
            MOCK_METHOD(uint8_t, timer, (), (const, override));
            MOCK_METHOD(uint16_t, sector_id, (), (const, override));
            MOCK_METHOD(const std::vector<Command>, commands, (), (const, override));
            MOCK_METHOD(void, set_triangles, (const std::vector<TransparentTriangle>&), (override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(void, set_position, (const DirectX::SimpleMath::Vector3&), (override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));

            std::shared_ptr<MockTrigger> with_number(uint32_t number)
            {
                ON_CALL(*this, number).WillByDefault(testing::Return(number));
                return shared_from_this();
            }

            std::shared_ptr<MockTrigger> with_room(uint32_t room)
            {
                ON_CALL(*this, room).WillByDefault(testing::Return(room));
                return shared_from_this();
            }

            std::shared_ptr<MockTrigger> with_commands(const std::vector<Command>& commands)
            {
                ON_CALL(*this, commands).WillByDefault(testing::Return(commands));
                return shared_from_this();
            }

            std::shared_ptr<MockTrigger> with_visible(bool visible)
            {
                ON_CALL(*this, visible).WillByDefault(testing::Return(visible));
                return shared_from_this();
            }

            std::shared_ptr<MockTrigger> with_visible(const std::function<bool()>& visible)
            {
                ON_CALL(*this, visible).WillByDefault(visible);
                return shared_from_this();
            }
        };
    }
}
