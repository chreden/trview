module;

#include <gmock/gmock.h>

export module trview.app:MockSettingsLoader;

import :ISettingsLoader;

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
