#include <trview.app/Settings/SettingsLoader.h>
#include <trview.common/Mocks/IFiles.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
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
            std::shared_ptr<IFiles> files{ mock_shared<MockFiles>() };

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

    std::unique_ptr<SettingsLoader> setup_setting(const std::string& setting, std::string randomizer_settings = "")
    {
        const auto contents = to_bytes(setting);
        auto files = mock_shared<MockFiles>();
        EXPECT_CALL(*files, appdata_directory).Times(2).WillRepeatedly(Return("appdata"));
        EXPECT_CALL(*files, load_file("appdata\\trview\\settings.txt")).Times(1).WillRepeatedly(Return(contents));
        EXPECT_CALL(*files, load_file("appdata\\trview\\randomizer.json")).Times(1).WillRepeatedly(Return(to_bytes(randomizer_settings)));
        return register_test_module().with_files(files).build();
    }

    std::unique_ptr<SettingsLoader> setup_save_setting(std::string& setting)
    {
        auto files = mock_shared<MockFiles>();
        EXPECT_CALL(*files, appdata_directory).Times(testing::AtLeast(1)).WillRepeatedly(Return("appdata"));
        EXPECT_CALL(*files, save_file(A<const std::string&>(), A<const std::string&>())).WillRepeatedly(SaveArg<1>(&setting));
        return register_test_module().with_files(files).build();
    }
}

TEST(SettingsLoader, FileNotFound)
{
    auto files = mock_shared<MockFiles>();
    EXPECT_CALL(*files, appdata_directory).Times(1).WillRepeatedly(Return("appdata"));
    EXPECT_CALL(*files, load_file("appdata\\trview\\settings.txt")).Times(1);
    auto loader = register_test_module().with_files(files).build();
    auto settings = loader->load_user_settings();

    UserSettings default_settings;
    ASSERT_EQ(settings, default_settings);
}

TEST(SettingsLoader, FileSaved)
{
    auto files = mock_shared<MockFiles>();
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
    auto loader = setup_setting("", "{\"fields\":{\"test1\":{\"type\":\"boolean\",\"default\":true}}}");
    auto settings = loader->load_user_settings();
    ASSERT_FALSE(settings.randomizer.settings.find("test1") == settings.randomizer.settings.end());
    auto setting = settings.randomizer.settings["test1"];
    ASSERT_EQ(std::get<bool>(setting.default_value), true);
}

TEST(SettingsLoader, MaxRecentFilesLoaded)
{
    auto loader = setup_setting("{\"maxrecentfiles\":15}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.max_recent_files, 15);
}

TEST(SettingsLoader, MaxRecentFilesSaved)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.max_recent_files = 15;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"maxrecentfiles\":15"));
}

TEST(SettingsLoader, RecentFilesTruncatedOnLoad)
{
    auto loader = setup_setting("{\"maxrecentfiles\":1,\"recent\":[\"a\",\"b\"]}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.max_recent_files, 1);
    const std::list<std::string> expected{ "a" };
    ASSERT_EQ(settings.recent_files, expected);
}

TEST(SettingsLoader, RecentFilesTruncatedOnSave)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.max_recent_files = 1;
    settings.recent_files = { "a", "b" };
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"maxrecentfiles\":1"));
    EXPECT_THAT(output, HasSubstr("\"recent\":[\"a\"]"));
}

TEST(SettingsLoader, MapColoursLoaded)
{
    auto loader = setup_setting("{\"mapcolours\":{\"special\":[[0,{\"a\":1.0,\"b\":0.0,\"g\":0.0,\"r\":1.0}]],\"colours\":[[8,{\"a\":1.0,\"b\":1.0,\"g\":0.0,\"r\":0.0}]]}}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.map_colours.colour(MapColours::Special::Default), Colour::Red);
    ASSERT_EQ(settings.map_colours.colour(SectorFlag::Death), Colour::Blue);
}

TEST(SettingsLoader, MapColoursSaved)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.map_colours.set_colour(MapColours::Special::Default, Colour::Red);
    settings.map_colours.set_colour(SectorFlag::Death, Colour::Blue);
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"special\":[[0,{\"a\":1.0,\"b\":0.0,\"g\":0.0,\"r\":1.0}]]"));
    EXPECT_THAT(output, HasSubstr("\"colours\":[[8,{\"a\":1.0,\"b\":1.0,\"g\":0.0,\"r\":0.0}]]"));
}

TEST(SettingsLoader, RouteColourSaved)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.route_colour = Colour::Yellow;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"routecolour\":{\"a\":1.0,\"b\":0.0,\"g\":1.0,\"r\":1.0}"));
}

TEST(SettingsLoader, WaypointColourSaved)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.waypoint_colour = Colour::Yellow;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"waypointcolour\":{\"a\":1.0,\"b\":0.0,\"g\":1.0,\"r\":1.0}"));
}

TEST(SettingsLoader, RouteColourLoaded)
{
    auto loader = setup_setting("{\"routecolour\":{\"a\":1.0,\"b\":1.0,\"g\":0.0,\"r\":0.0}}");
    auto settings_blue = loader->load_user_settings();
    ASSERT_EQ(settings_blue.route_colour, Colour::Blue);

    auto loader_yellow = setup_setting("{\"routecolour\":{\"a\":1.0,\"b\":0.0,\"g\":1.0,\"r\":1.0}}");
    auto settings_yellow = loader_yellow->load_user_settings();
    ASSERT_EQ(settings_yellow.route_colour, Colour::Yellow);
}

TEST(SettingsLoader, WaypointColourLoaded)
{
    auto loader = setup_setting("{\"waypointcolour\":{\"a\":1.0,\"b\":1.0,\"g\":0.0,\"r\":0.0}}");
    auto settings_blue = loader->load_user_settings();
    ASSERT_EQ(settings_blue.waypoint_colour, Colour::Blue);

    auto loader_yellow = setup_setting("{\"waypointcolour\":{\"a\":1.0,\"b\":0.0,\"g\":1.0,\"r\":1.0}}");
    auto settings_yellow = loader_yellow->load_user_settings();
    ASSERT_EQ(settings_yellow.waypoint_colour, Colour::Yellow);
}

TEST(SettingsLoader, RouteStartupLoaded)
{
    auto loader = setup_setting("{\"routestartup\":false}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.route_startup, false);

    auto loader_true = setup_setting("{\"routestartup\":true}");
    auto settings_true = loader_true->load_user_settings();
    ASSERT_EQ(settings_true.route_startup, true);
}

TEST(SettingsLoader, RouteStartupSaved)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.route_startup = false;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"routestartup\":false"));

    settings.route_startup = true;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"routestartup\":true"));
}

TEST(SettingsLoader, RecentRoutesLoaded)
{
    auto loader = setup_setting("{\"recentroutes\":{\"path.tr2\":{\"is_rando\":true,\"route_path\":\"route.tvr\"}}}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.recent_routes["path.tr2"].route_path, "route.tvr");
    ASSERT_EQ(settings.recent_routes["path.tr2"].is_rando, true);
}

TEST(SettingsLoader, RecentRoutesSaved)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.recent_routes["path.tr2"] = { "route.tvr", true };
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"recentroutes\":{\"path.tr2\":{\"is_rando\":true,\"route_path\":\"route.tvr\"}}"));
}

TEST(SettingsLoader, FovLoaded)
{
    auto loader = setup_setting("{\"fov\":10.0}");
    auto settings = loader->load_user_settings();
    ASSERT_EQ(settings.fov, 10.0f);
}

TEST(SettingsLoader, FovSaved)
{
    std::string output;
    auto loader = setup_save_setting(output);
    UserSettings settings;
    settings.fov = 10.0f;
    loader->save_user_settings(settings);
    EXPECT_THAT(output, HasSubstr("\"fov\":10.0"));
}
