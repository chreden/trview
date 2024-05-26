#pragma once

#include "../../Windows/Statics/IStaticsWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockStaticsWindow : public IStaticsWindow
        {
            MockStaticsWindow();
            virtual ~MockStaticsWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(std::weak_ptr<IStaticMesh>, selected_static, (), (const, override));
            MOCK_METHOD(void, set_current_room, (const std::weak_ptr<IRoom>&), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, set_selected_static, (const std::weak_ptr<IStaticMesh>&), (override));
            MOCK_METHOD(void, set_statics, (const std::vector<std::weak_ptr<IStaticMesh>>&), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
