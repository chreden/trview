#include <trview.app/Application.h>
#include <trview.app/Mocks/Menus/IUpdateChecker.h>
#include <trview.app/Mocks/Settings/ISettingsLoader.h>

using namespace trview;
using namespace trview::tests;

TEST(Application, ChecksForUpdates)
{
    auto [update_checker_ptr, update_checker] = create_mock<mocks::MockUpdateChecker>();
    EXPECT_CALL(update_checker, check_for_updates).Times(1);
    CoInitialize(nullptr);
    Application application(create_test_window(L"ApplicationTests"), std::move(update_checker_ptr), std::make_unique<mocks::MockSettingsLoader>(), std::wstring());
}

TEST(Application, SettingsLoadedAndSaved)
{
    auto [settings_loader_ptr, settings_loader] = create_mock<mocks::MockSettingsLoader>();
    EXPECT_CALL(settings_loader, load_user_settings).Times(1);
    EXPECT_CALL(settings_loader, save_user_settings).Times(1);
    CoInitialize(nullptr);
    Application application(create_test_window(L"ApplicationTests"), std::make_unique<mocks::MockUpdateChecker>(), std::move(settings_loader_ptr), std::wstring());
}