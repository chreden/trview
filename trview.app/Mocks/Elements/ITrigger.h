#pragma once

#include "../../Elements/ITrigger.h"

namespace trview
{
    namespace mocks
    {
        struct MockTrigger : public ITrigger, public std::enable_shared_from_this<MockTrigger>
        {
            virtual ~MockTrigger() = default;
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&));
            MOCK_METHOD(bool, visible, (), (const));
            MOCK_METHOD(void, set_visible, (bool));
            MOCK_METHOD(uint32_t, number, (), (const));
            MOCK_METHOD(uint32_t, room, (), (const));
            MOCK_METHOD(uint16_t, x, (), (const));
            MOCK_METHOD(uint16_t, z, (), (const));
            MOCK_METHOD(bool, triggers_item, (uint32_t index), (const));
            MOCK_METHOD(TriggerType, type, (), (const));
            MOCK_METHOD(bool, only_once, (), (const));
            MOCK_METHOD(uint16_t, flags, (), (const));
            MOCK_METHOD(uint8_t, timer, (), (const));
            MOCK_METHOD(uint16_t, sector_id, (), (const));
            MOCK_METHOD(const std::vector<Command>, commands, (), (const));
            MOCK_METHOD(const std::vector<TransparentTriangle>&, triangles, (), (const));
            MOCK_METHOD(void, set_triangles, (const std::vector<TransparentTriangle>&));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const));
            MOCK_METHOD(void, set_position, (const DirectX::SimpleMath::Vector3&));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const));

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
