#pragma once

#include "../../Windows/IWindows.h"

namespace trview
{
    namespace mocks
    {
        struct MockWindows : public IWindows
        {
            MockWindows();
            virtual ~MockWindows();
            MOCK_METHOD(bool, is_route_window_open, (), (const, override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, select, (const std::weak_ptr<ICameraSink>&), (override));
            MOCK_METHOD(void, select, (const std::weak_ptr<IItem>&), (override));
            MOCK_METHOD(void, select, (const std::weak_ptr<ILight>&), (override));
            MOCK_METHOD(void, select, (const std::weak_ptr<ISoundSource>&), (override));
            MOCK_METHOD(void, select, (const std::weak_ptr<IStaticMesh>&), (override));
            MOCK_METHOD(void, select, (const std::weak_ptr<ITrigger>&), (override));
            MOCK_METHOD(void, select, (const std::weak_ptr<IWaypoint>&), (override));
            MOCK_METHOD(void, set_level, (const std::weak_ptr<ILevel>&), (override));
            MOCK_METHOD(void, set_room, (const std::weak_ptr<IRoom>&), (override));
            MOCK_METHOD(void, set_route, (const std::weak_ptr<IRoute>&), (override));
            MOCK_METHOD(void, set_settings, (const UserSettings&), (override));
            MOCK_METHOD(void, setup, (const UserSettings&), (override));
        };
    }
}
