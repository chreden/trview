#pragma once

#include "../../Windows/IWindows.h"

namespace trview
{
    namespace mocks
    {
        struct MockWindows : public IWindows
        {
            MockWindows();
            ~MockWindows();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_item_visibility, (const Item&, bool), (override));
            MOCK_METHOD(void, set_level, (std::shared_ptr<ILevel>), (override));
            MOCK_METHOD(void, set_room, (uint32_t), (override));
            MOCK_METHOD(void, set_selected_camera_sink, (std::weak_ptr<ICameraSink>), (override));
            MOCK_METHOD(void, set_selected_item, (const Item&), (override));
            MOCK_METHOD(void, set_selected_light, (std::weak_ptr<ILight>), (override));
            MOCK_METHOD(void, set_selected_trigger, (std::weak_ptr<ITrigger>), (override));
            MOCK_METHOD(void, set_settings, (const UserSettings&), (override));
            MOCK_METHOD(void, startup, (const UserSettings&), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
