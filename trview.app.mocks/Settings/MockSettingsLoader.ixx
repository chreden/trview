module;

#include <gmock/gmock.h>

export module trview.app.mocks:MockSettingsLoader;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockSettingsLoader : public ISettingsLoader
        {
            MOCK_METHOD(UserSettings, load_user_settings, (), (const, override));
            MOCK_METHOD(void, save_user_settings, (const UserSettings&), (override));
        };
    }
}
