#pragma once

#include "../../Settings/ISettingsLoader.h"

namespace trview
{
    namespace mocks
    {
        class MockSettingsLoader final : public ISettingsLoader
        {
        public:
            virtual ~MockSettingsLoader() = default;
            MOCK_METHOD(UserSettings, load_user_settings, (), (const));
            MOCK_METHOD(void, save_user_settings, (const UserSettings&));
        };
    }
}
