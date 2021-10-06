#pragma once

#include <trview.app/UI/IContextMenu.h>

namespace trview
{
    namespace mocks
    {
        struct MockContextMenu final : public IContextMenu
        {
            virtual ~MockContextMenu() = default;
            MOCK_METHOD(void, set_position, (const Point&), (override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(void, set_remove_enabled, (bool), (override));
            MOCK_METHOD(void, set_hide_enabled, (bool), (override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(void, set_mid_waypoint_enabled, (bool), (override));
            MOCK_METHOD(void, set_randomizer_tools, (bool), (override));
        };
    }
}
