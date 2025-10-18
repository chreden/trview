#pragma once

#include "../../Windows/Statics/IStaticsWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockStaticsWindowManager : public IStaticsWindowManager
        {
            MockStaticsWindowManager();
            ~MockStaticsWindowManager();
            MOCK_METHOD(std::weak_ptr<IStaticsWindow>, create_window, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_room, (const std::weak_ptr<IRoom>&), (override));
            MOCK_METHOD(void, set_statics, (const std::vector<std::weak_ptr<IStaticMesh>>&), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
