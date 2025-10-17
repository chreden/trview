#pragma once

#include <trview.app/Windows/ILightsWindowManager.h>

namespace trview
{
    namespace mocks
    {
        struct MockLightsWindowManager : public ILightsWindowManager
        {
            MockLightsWindowManager();
            virtual ~MockLightsWindowManager();
            MOCK_METHOD(void, set_lights, (const std::vector<std::weak_ptr<ILight>>&), (override));
            MOCK_METHOD(std::weak_ptr<ILightsWindow>, create_window, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, set_selected_light, (const std::weak_ptr<ILight>&), (override));
            MOCK_METHOD(void, set_level_version, (trlevel::LevelVersion), (override));
            MOCK_METHOD(void, set_room, (const std::weak_ptr<IRoom>&), (override));
        };
    }
}
