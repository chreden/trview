#include <trview.app/Application.h>
#include <trview.app/Mocks/Menus/IUpdateChecker.h>
#include <trview.app/Mocks/Settings/ISettingsLoader.h>
#include <trview.app/Mocks/Menus/IFileDropper.h>
#include <trview.app/Mocks/Menus/ILevelSwitcher.h>
#include <trlevel/Mocks/ILevelLoader.h>

using namespace trview;
using namespace trview::tests;
using namespace testing;
using namespace trview::mocks;
using namespace trlevel::mocks;

TEST(Application, ChecksForUpdates)
{
    auto [update_checker_ptr, update_checker] = create_mock<MockUpdateChecker>();
    EXPECT_CALL(update_checker, check_for_updates).Times(1);
    CoInitialize(nullptr);
    Application application(create_test_window(L"ApplicationTests"),
        std::move(update_checker_ptr),
        std::make_unique<MockSettingsLoader>(),
        std::make_unique<MockFileDropper>(),
        std::make_unique<MockLevelLoader>(),
        std::make_unique<MockLevelSwitcher>(),
        std::wstring());
}

TEST(Application, SettingsLoadedAndSaved)
{
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    EXPECT_CALL(settings_loader, load_user_settings).Times(1);
    EXPECT_CALL(settings_loader, save_user_settings).Times(1);
    CoInitialize(nullptr);
    Application application(create_test_window(L"ApplicationTests"),
        std::make_unique<MockUpdateChecker>(),
        std::move(settings_loader_ptr),
        std::make_unique<MockFileDropper>(),
        std::make_unique<MockLevelLoader>(),
        std::make_unique<MockLevelSwitcher>(),
        std::wstring());
}

TEST(Application, FileDropperOpensFile)
{
    auto [file_dropper_ptr, file_dropper] = create_mock<MockFileDropper>();
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    CoInitialize(nullptr);

    EXPECT_CALL(level_loader, load_level("test_path.tr2"))
        .Times(1)
        .WillRepeatedly(Throw(std::exception()));

    Application application(create_test_window(L"ApplicationTests"),
        std::make_unique<MockUpdateChecker>(),
        std::make_unique<MockSettingsLoader>(),
        std::move(file_dropper_ptr),
        std::move(level_loader_ptr),
        std::make_unique<MockLevelSwitcher>(),
        std::wstring());
    file_dropper.on_file_dropped("test_path.tr2");
}

TEST(Application, LevelLoadedOnSwitchLevel)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    auto [level_switcher_ptr, level_switcher] = create_mock<MockLevelSwitcher>();

    EXPECT_CALL(level_loader, load_level("test_path.tr2"))
        .Times(1)
        .WillRepeatedly(Throw(std::exception()));

    Application application(create_test_window(L"ApplicationTests"),
        std::make_unique<MockUpdateChecker>(),
        std::make_unique<MockSettingsLoader>(),
        std::make_unique<MockFileDropper>(),
        std::move(level_loader_ptr),
        std::move(level_switcher_ptr),
        std::wstring());

    level_switcher.on_switch_level("test_path.tr2");
}
