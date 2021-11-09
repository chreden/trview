#include <trview.app/Settings/SettingsLoader.h>
#include <trview.common/Mocks/IFiles.h>

using namespace trview;
using testing::Return;
using testing::A;
using testing::SaveArg;
using testing::HasSubstr;

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

    std::unique_ptr<SettingsLoader> setup_save_setting(std::string& setting)
    {
        auto files = std::make_shared<MockFiles>();
        EXPECT_CALL(*files, appdata_directory).Times(testing::AtLeast(1)).WillRepeatedly(Return("appdata"));
        EXPECT_CALL(*files, save_file(A<const std::string&>(), A<const std::string&>())).WillRepeatedly(SaveArg<1>(&setting));
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
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.recent_files = { "a", "b", "c" };
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"recent\":[\"a\",\"b\",\"c\"]"));
}

TEST(SettingsLoader, CameraSensitivityLoaded)
{
    auto loader = setup_setting("{\"camera\":0.5}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.camera_sensitivity, 0.5f);
}

TEST(SettingsLoader, CameraSensitivitySaved)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.camera_sensitivity = 0.5f;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"camera\":0.5"));
}

TEST(SettingsLoader, CameraMovementSpeedLoaded)
{
    auto loader = setup_setting("{\"movement\":1.2}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.camera_movement_speed, 1.2f);
}

TEST(SettingsLoader, CameraMovementSpeedSaved)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.camera_movement_speed = 1.2f;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"movement\":1.2"));
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
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.vsync = false;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"vsync\":false"));

    settings.vsync = true;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"vsync\":true"));
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
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.go_to_lara = false;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"gotolara\":false"));

    settings.go_to_lara = true;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"gotolara\":true"));
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
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.invert_map_controls = false;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"invertmapcontrols\":false"));

    settings.invert_map_controls = true;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"invertmapcontrols\":true"));
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
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.items_startup = false;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"itemsstartup\":false"));

    settings.items_startup = true;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"itemsstartup\":true"));
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
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.triggers_startup = false;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"triggersstartup\":false"));

    settings.triggers_startup = true;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"triggersstartup\":true"));
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
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.auto_orbit = false;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"autoorbit\":false"));

    settings.auto_orbit = true;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"autoorbit\":true"));
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
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.invert_vertical_pan = false;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"invertverticalpan\":false"));

    settings.invert_vertical_pan = true;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"invertverticalpan\":true"));
}

TEST(SettingsLoader, BackgroundColourLoaded)
{
    auto loader = setup_setting("{\"background\":12345}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.background_colour, 12345);
}

TEST(SettingsLoader, BackgroundColourSaved)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.background_colour = 12345;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"background\":12345"));
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
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.rooms_startup = false;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"roomsstartup\":false"));

    settings.rooms_startup = true;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"roomsstartup\":true"));
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
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.camera_acceleration = false;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"cameraacceleration\":false"));

    settings.camera_acceleration = true;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"cameraacceleration\":true"));
}

TEST(SettingsLoader, CameraAccelerationRateLoaded)
{
    auto loader = setup_setting("{\"cameraaccelerationrate\":1.2}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.camera_acceleration_rate, 1.2f);
}

TEST(SettingsLoader, CameraAccelerationRateSaved)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.camera_acceleration_rate = 1.2f;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"cameraaccelerationrate\":1.2"));
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
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.camera_display_degrees = false;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"cameradisplaydegrees\":false"));

    settings.camera_display_degrees = true;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"cameradisplaydegrees\":true"));
}

TEST(SettingsLoader, RandomizerToolsLoaded)
{
    auto loader = setup_setting("{\"randomizertools\":false}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.randomizer_tools, false);

    auto loader_true = setup_setting("{\"randomizertools\":true}");
    auto settings_true = loader_true->load_user_settings();
    ASSERT_EQ(settings_true.randomizer_tools, true);
}

TEST(SettingsLoader, RandomizerToolsSaved)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.randomizer_tools = false;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"randomizertools\":false"));

    settings.randomizer_tools = true;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"randomizertools\":true"));
}

TEST(SettingsLoader, RandomizerSettingsLoaded)
{
    auto loader = setup_setting("{\"randomizer\":{\"fields\":{\"test1\":{\"type\":\"boolean\",\"default\":true}}}}");
    auto settings = loader->load_user_settings();
    ASSERT_FALSE(settings.randomizer.settings.find("test1") == settings.randomizer.settings.end());
    auto setting = settings.randomizer.settings["test1"];
    ASSERT_EQ(std::get<bool>(setting.default_value), true);
}

TEST(SettingsLoader, RandomizerSettingsSaved)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.randomizer.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::Boolean, true };

    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"randomizer\":{\"fields\":{\"test1\":{\"default\":true,\"display\":\"Test 1\",\"type\":\"boolean\"}}}"));
}
