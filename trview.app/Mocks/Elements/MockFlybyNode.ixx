module;

#include <gmock/gmock.h>
#include <SimpleMath.h>

export module trview.app:MockFlybyNode;

import :IFlybyNode;

namespace trview
{
    namespace mocks
    {
        export struct MockFlybyNode : public IFlybyNode
        {
            explicit MockFlybyNode() {}
            virtual ~MockFlybyNode() {}
            MOCK_METHOD(DirectX::SimpleMath::Vector3, direction, (), (const, override));
            MOCK_METHOD(uint16_t, flags, (), (const, override));
            MOCK_METHOD(std::weak_ptr<IFlyby>, flyby, (), (const, override));
            MOCK_METHOD(uint16_t, fov, (), (const, override));
            MOCK_METHOD(std::weak_ptr<ILevel>, level, (), (const, override));
            MOCK_METHOD(int32_t, number, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(int16_t, roll, (), (const, override));
            MOCK_METHOD(uint32_t, room, (), (const, override));
            MOCK_METHOD(uint16_t, speed, (), (const, override));
            MOCK_METHOD(uint16_t, timer, (), (const, override));
            MOCK_METHOD(int32_t, filterable_index, (), (const, override));
        };
    }
}
