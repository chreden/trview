#pragma once

#include "../../Lua/Scriptable/IScriptable.h"

namespace trview
{
    struct ICamera;

    namespace mocks
    {
        struct MockScriptable : public IScriptable
        {
            MockScriptable();
            ~MockScriptable();
            MOCK_METHOD(void, click, (), (override));
            MOCK_METHOD(int, data, (), (const, override));
            MOCK_METHOD(std::shared_ptr<IMesh>, mesh, (), (const, override));
            MOCK_METHOD(std::string, notes, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(void, render, (const ICamera&), (override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, screen_position, (), (const, override));
            MOCK_METHOD(void, set_data, (int), (override));
            MOCK_METHOD(void, set_notes, (const std::string&), (override));
            MOCK_METHOD(void, set_on_click, (int), (override));
            MOCK_METHOD(void, set_position, (const DirectX::SimpleMath::Vector3&), (override));
            MOCK_METHOD(void, set_on_tooltip, (int), (override));
            MOCK_METHOD(void, set_screen_position, (const DirectX::SimpleMath::Vector3&), (override));
            MOCK_METHOD(std::string, tooltip, (), (const, override));
        };
    }
}
