#include <trview.app/Settings/SettingsLoader.h>
#include <trview.common/Mocks/IFiles.h>

using namespace trview;
using testing::Return;
using testing::A;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IFiles> files{ std::make_shared<MockFiles>() };

            std::unique_ptr<SettingsLoader> build()
            {
                return std::make_unique<SettingsLoader>(files);
            }

            test_module& with_files(const std::shared_ptr<IFiles>& files)
            {
                this->files = files;
                return *this;
            }
        };

        return test_module{};
    };

    std::vector<uint8_t> to_bytes(const std::string& value)
    {
        return std::vector<uint8_t>(value.begin(), value.end());
    }

    std::unique_ptr<SettingsLoader> setup_setting(const std::string& setting)
    {
        const auto contents = to_bytes(setting);
        auto files = std::make_shared<MockFiles>();
        EXPECT_CALL(*files, appdata_directory).Times(1).WillRepeatedly(Return("appdata"));
        EXPECT_CALL(*files, load_file("appdata\\trview\\settings.txt")).Times(1).WillRepeatedly(Return(contents));
        return register_test_module().with_files(files).build();
    }
}

TEST(SettingsLoader, FileNotFound)
{
    auto files = std::make_shared<MockFiles>();
    EXPECT_CALL(*files, appdata_directory).Times(1).WillRepeatedly(Return("appdata"));
    EXPECT_CALL(*files, load_file("appdata\\trview\\settings.txt")).Times(1);
    auto loader = register_test_module().with_files(files).build();
    auto settings = loader->load_user_settings();

    UserSettings default_settings;
    ASSERT_EQ(settings, default_settings);
}

TEST(SettingsLoader, FileSaved)
{
    auto files = std::make_shared<MockFiles>();
    EXPECT_CALL(*files, appdata_directory).Times(1).WillRepeatedly(Return("appdata"));
    EXPECT_CALL(*files, save_file("appdata\\trview\\settings.txt", A<const std::string&>())).Times(1);
    auto loader = register_test_module().with_files(files).build();

    UserSettings settings;
    loader->save_user_settings(settings);
}

TEST(SettingsLoader, RecentFilesLoaded)
{
    auto loader = setup_setting("{\"recent\":[\"a\", \"b\", \"c\"]}");
    auto settings = loader->load_user_settings();
    const std::list<std::string> expected{ "a", "b", "c" };
    ASSERT_EQ(settings.recent_files, expected);
}

TEST(SettingsLoader, RecentFilesSaved)
{
    FAIL();
}

TEST(SettingsLoader, CameraSensitivityLoaded)
{
    auto loader = setup_setting("{\"camera\":0.5}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.camera_sensitivity, 0.5f);
}

TEST(SettingsLoader, CameraSensitivitySaved)
{
    FAIL();
}

TEST(SettingsLoader, CameraMovementSpeedLoaded)
{
    auto loader = setup_setting("{\"movement\":1.2}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.camera_movement_speed, 1.2f);
}

TEST(SettingsLoader, CameraMovementSpeedSaved)
{
    FAIL();
}

TEST(SettingsLoader, VsyncLoaded)
{
    auto loader = setup_setting("{\"vsync\":false}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.vsync, false);

    auto loader_true = setup_setting("{\"vsync\":true}");
    auto settings_true = loader_true->load_user_settings();
    ASSERT_EQ(settings_true.vsync, true);
}

TEST(SettingsLoader, VsyncSaved)
{
    FAIL();
}

TEST(SettingsLoader, GoToLaraLoaded)
{
    auto loader = setup_setting("{\"gotolara\":false}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.go_to_lara, false);

    auto loader_true = setup_setting("{\"gotolara\":true}");
    auto settings_true = loader_true->load_user_settings();
    ASSERT_EQ(settings_true.go_to_lara, true);
}

TEST(SettingsLoader, GoToLaraSaved)
{
    FAIL();
}

TEST(SettingsLoader, InvertMapControlsLoaded)
{
    auto loader = setup_setting("{\"invertmapcontrols\":false}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.invert_map_controls, false);

    auto loader_true = setup_setting("{\"invertmapcontrols\":true}");
    auto settings_true = loader_true->load_user_settings();
    ASSERT_EQ(settings_true.invert_map_controls, true);
}

TEST(SettingsLoader, InvertMapControlsSaved)
{
    FAIL();
}

TEST(SettingsLoader, ItemsStartupLoaded)
{
    auto loader = setup_setting("{\"itemsstartup\":false}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.items_startup, false);

    auto loader_true = setup_setting("{\"itemsstartup\":true}");
    auto settings_true = loader_true->load_user_settings();
    ASSERT_EQ(settings_true.items_startup, true);
}

TEST(SettingsLoader, ItemsStartupSaved)
{
    FAIL();
}

TEST(SettingsLoader, TriggersStartupLoaded)
{
    auto loader = setup_setting("{\"triggerstartup\":false}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.triggers_startup, false);

    auto loader_true = setup_setting("{\"triggersstartup\":true}");
    auto settings_true = loader_true->load_user_settings();
    ASSERT_EQ(settings_true.triggers_startup, true);
}

TEST(SettingsLoader, TriggersStartupSaved)
{
    FAIL();
}

TEST(SettingsLoader, AutoOrbitLoaded)
{
    auto loader = setup_setting("{\"autoorbit\":false}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.auto_orbit, false);

    auto loader_true = setup_setting("{\"autoorbit\":true}");
    auto settings_true = loader_true->load_user_settings();
    ASSERT_EQ(settings_true.auto_orbit, true);
}

TEST(SettingsLoader, AutoOrbitSaved)
{
    FAIL();
}

TEST(SettingsLoader, InvertVerticalPanLoaded)
{
    auto loader = setup_setting("{\"invertverticalpan\":false}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.invert_vertical_pan, false);

    auto loader_true = setup_setting("{\"invertverticalpan\":true}");
    auto settings_true = loader_true->load_user_settings();
    ASSERT_EQ(settings_true.invert_vertical_pan, true);
}

TEST(SettingsLoader, InvertVerticalPanSaved)
{
    FAIL();
}

TEST(SettingsLoader, BackgroundColourLoaded)
{
    auto loader = setup_setting("{\"background\":12345}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.background_colour, 12345);
}

TEST(SettingsLoader, BackgroundColourSaved)
{
    FAIL();
}

TEST(SettingsLoader, RoomsStartupLoaded)
{
    auto loader = setup_setting("{\"roomsstartup\":false}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.rooms_startup, false);

    auto loader_true = setup_setting("{\"roomsstartup\":true}");
    auto settings_true = loader_true->load_user_settings();
    ASSERT_EQ(settings_true.rooms_startup, true);
}

TEST(SettingsLoader, RoomsStartupSaved)
{
    FAIL();
}

TEST(SettingsLoader, CameraAccelerationLoaded)
{
    auto loader = setup_setting("{\"cameraacceleration\":false}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.camera_acceleration, false);

    auto loader_true = setup_setting("{\"cameraacceleration\":true}");
    auto settings_true = loader_true->load_user_settings();
    ASSERT_EQ(settings_true.camera_acceleration, true);
}

TEST(SettingsLoader, CameraAccelerationSaved)
{
    FAIL();
}

TEST(SettingsLoader, CameraAccelerationRateLoaded)
{
    auto loader = setup_setting("{\"cameraaccelerationrate\":1.2}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.camera_acceleration_rate, 1.2f);
}

TEST(SettingsLoader, CameraAccelerationRateSaved)
{
    FAIL();
}

TEST(SettingsLoader, CameraDisplayDegreesLoaded)
{
    auto loader = setup_setting("{\"cameradisplaydegrees\":false}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.camera_display_degrees, false);

    auto loader_true = setup_setting("{\"cameradisplaydegrees\":true}");
    auto settings_true = loader_true->load_user_settings();
    ASSERT_EQ(settings_true.camera_display_degrees, true);
}

TEST(SettingsLoader, CameraDisplayDegreesSaved)
{
    FAIL();
}
