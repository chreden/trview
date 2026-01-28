#pragma once

#include "../../Elements/Flyby/IFlyby.h"

namespace trview
{
    namespace mocks
    {
        struct MockFlyby : public IFlyby
        {
            explicit MockFlyby();
            virtual ~MockFlyby();
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(std::vector<std::weak_ptr<IFlybyNode>>, nodes, (), (const, override));
            MOCK_METHOD(std::weak_ptr<ILevel>, level, (), (const, override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(void, render, (const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(CameraState, update_state, (const CameraState&, float), (const, override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(int32_t, filterable_index, (), (const, override));
        };
    }
}
