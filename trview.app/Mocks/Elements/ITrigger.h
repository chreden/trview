#pragma once

#include "../../Elements/ITrigger.h"

namespace trview
{
    namespace mocks
    {
        struct MockTrigger final : public ITrigger
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
        };
    }
}
