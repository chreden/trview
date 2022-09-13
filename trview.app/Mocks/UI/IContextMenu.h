#pragma once

#include "../../UI/IContextMenu.h"

namespace trview
{
    namespace mocks
    {
        struct MockContextMenu : public IContextMenu
        {
            virtual ~MockContextMenu() = default;
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(void, set_remove_enabled, (bool), (override));
            MOCK_METHOD(void, set_hide_enabled, (bool), (override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(void, set_mid_waypoint_enabled, (bool), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_triggered_by, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
        };
    }
}
