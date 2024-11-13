#include <trview.app/UI/ViewerUI.h>
#include <trview.app/Windows/IViewer.h>
#include <trview.app/Mocks/Graphics/ITextureStorage.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.app/Mocks/UI/IMapRenderer.h>
#include <trview.tests.common/Window.h>
#include <trview.app/Mocks/UI/ISettingsWindow.h>
#include <trview.app/Mocks/UI/IViewOptions.h>
#include <trview.app/Mocks/UI/IContextMenu.h>
#include <trview.app/Mocks/UI/ICameraControls.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include <trview.app/Mocks/Tools/IToolbar.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

namespace
{
    Event<> shortcut_handler;

    auto register_test_module()
    {
        struct test_module
        {
            trview::Window window{ create_test_window(L"ViewerUITests") };
            std::shared_ptr<ITextureStorage> texture_storage{ mock_shared<MockTextureStorage>() };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            IMapRenderer::Source map_renderer_source{ [](auto&&...) { return mock_unique<MockMapRenderer>(); }};
            std::unique_ptr<ISettingsWindow> settings_window{ mock_unique<MockSettingsWindow>() };
            std::unique_ptr<IViewOptions> view_options{ mock_unique<MockViewOptions>() };
            std::unique_ptr<trview::IContextMenu> context_menu{ mock_unique<MockContextMenu>() };
            std::unique_ptr<ICameraControls> camera_controls{ mock_unique<MockCameraControls>() };
            std::unique_ptr<IToolbar> toolbar{ mock_unique<MockToolbar>() };

            std::unique_ptr<ViewerUI> build()
            {
                EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
                return std::make_unique<ViewerUI>(window, texture_storage, shortcuts, map_renderer_source, std::move(settings_window), std::move(view_options), std::move(context_menu), std::move(camera_controls), std::move(toolbar));
            }

            test_module& with_settings_window(std::unique_ptr<ISettingsWindow> window)
            {
                settings_window = std::move(window);
                return *this;
            }

            test_module& with_view_options(std::unique_ptr<IViewOptions> source)
            {
                view_options = std::move(source);
                return *this;
            }

            test_module& with_context_menu(std::unique_ptr<trview::IContextMenu> menu)
            {
                context_menu = std::move(menu);
                return *this;
            }

            test_module& with_camera_controls(std::unique_ptr<ICameraControls> controls)
            {
                camera_controls = std::move(controls);
                return *this;
            }

            test_module& with_map_renderer_source(IMapRenderer::Source map_renderer_source)
            {
                this->map_renderer_source = map_renderer_source;
                return *this;
            }
        };
        return test_module{};
    }
}

TEST(ViewerUI, OnCameraDisplayDegreesEventRaised)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto ui = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    std::optional<UserSettings> settings;
    auto token = ui->on_settings += [&](const auto& value)
    {
        settings = value;
    };

    settings_window.on_camera_display_degrees(true);
    ASSERT_TRUE(settings.has_value());
    ASSERT_TRUE(settings.value().camera_display_degrees);
    settings.reset();

    settings_window.on_camera_display_degrees(false);
    ASSERT_TRUE(settings.has_value());
    ASSERT_FALSE(settings.value().camera_display_degrees);
}

TEST(ViewerUI, BoundingBoxUpdatesViewOptions)
{
    auto [view_options_ptr, view_options] = create_mock<MockViewOptions>();
    auto ui = register_test_module().with_view_options(std::move(view_options_ptr)).build();

    EXPECT_CALL(view_options, set_toggle(IViewer::Options::show_bounding_boxes, true)).Times(1);

    ui->set_toggle(IViewer::Options::show_bounding_boxes, true);
}

TEST(ViewerUI, ShowBoundingBoxesEventRaised)
{
    auto [view_options_ptr, view_options] = create_mock<MockViewOptions>();
    auto ui = register_test_module().with_view_options(std::move(view_options_ptr)).build();

    std::optional<std::tuple<std::string, bool>> show;
    auto token = ui->on_toggle_changed += [&](const auto& name, const auto& value)
    {
        show = { name, value };
    };

    view_options.on_toggle_changed(IViewer::Options::show_bounding_boxes, true);
    ASSERT_TRUE(show.has_value());
    ASSERT_EQ(std::get<0>(show.value()), IViewer::Options::show_bounding_boxes);
    ASSERT_TRUE(std::get<1>(show.value()));
}

TEST(ViewerUI, SetMidWaypointEnabled)
{
    auto [context_menu_ptr, context_menu] = create_mock<MockContextMenu>();
    EXPECT_CALL(context_menu, set_mid_waypoint_enabled).Times(1);
    auto ui = register_test_module().with_context_menu(std::move(context_menu_ptr)).build();

    ui->set_mid_waypoint_enabled(true);
}

TEST(ViewerUI, OnAddMidWaypoint)
{
    auto [context_menu_ptr, context_menu] = create_mock<MockContextMenu>();
    auto ui = register_test_module().with_context_menu(std::move(context_menu_ptr)).build();

    bool raised = false;
    auto token = ui->on_add_mid_waypoint += [&]()
    {
        raised = true;
    };

    context_menu.on_add_mid_waypoint();
    ASSERT_TRUE(raised);
}

TEST(ViewerUI, OnRandomizerToolsEventRaised)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto ui = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    std::optional<UserSettings> settings;
    auto token = ui->on_settings += [&](const auto& value)
    {
        settings = value;
    };

    settings_window.on_randomizer_tools(true);
    ASSERT_TRUE(settings.has_value());
    ASSERT_TRUE(settings.value().randomizer_tools);
    settings.reset();

    settings_window.on_randomizer_tools(false);
    ASSERT_TRUE(settings.has_value());
    ASSERT_FALSE(settings.value().randomizer_tools);
}

TEST(ViewerUI, CameraControlsResetEventRaised)
{
    auto [camera_controls_ptr, camera_controls] = create_mock<MockCameraControls>();
    auto ui = register_test_module().with_camera_controls(std::move(camera_controls_ptr)).build();

    bool raised = false;
    auto token = ui->on_camera_reset += [&]()
    {
        raised = true;
    };

    camera_controls.on_reset();
    ASSERT_TRUE(raised);
}

TEST(ViewerUI, CameraControlsModeSelectedEventRaised)
{
    auto [camera_controls_ptr, camera_controls] = create_mock<MockCameraControls>();
    auto ui = register_test_module().with_camera_controls(std::move(camera_controls_ptr)).build();

    std::optional<ICamera::Mode> raised;
    auto token = ui->on_camera_mode += [&](auto mode)
    {
        raised = mode;
    };

    camera_controls.on_mode_selected(ICamera::Mode::Free);
    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), ICamera::Mode::Free);
}

TEST(ViewerUI, CameraControlsProjectionModeSelectedEventRaised)
{
    auto [camera_controls_ptr, camera_controls] = create_mock<MockCameraControls>();
    auto ui = register_test_module().with_camera_controls(std::move(camera_controls_ptr)).build();

    std::optional<ProjectionMode> raised;
    auto token = ui->on_camera_projection_mode += [&](auto mode)
    {
        raised = mode;
    };

    camera_controls.on_projection_mode_selected(ProjectionMode::Orthographic);
    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), ProjectionMode::Orthographic);
}

TEST(ViewerUI, SetCameraModeUpdatesCameraControls)
{
    auto [camera_controls_ptr, camera_controls] = create_mock<MockCameraControls>();
    auto ui = register_test_module().with_camera_controls(std::move(camera_controls_ptr)).build();
    EXPECT_CALL(camera_controls, set_mode(ICamera::Mode::Orbit)).Times(1);

    ui->set_camera_mode(ICamera::Mode::Orbit);
}

TEST(ViewerUI, SetCameraProjectionModeUpdatesCameraControls)
{
    auto [camera_controls_ptr, camera_controls] = create_mock<MockCameraControls>();
    auto ui = register_test_module().with_camera_controls(std::move(camera_controls_ptr)).build();
    EXPECT_CALL(camera_controls, set_projection_mode(ProjectionMode::Orthographic)).Times(1);

    ui->set_camera_projection_mode(ProjectionMode::Orthographic);
}

TEST(ViewerUI, SetMapColoursUpdatesMapRenderer)
{
    auto [map_renderer_ptr, map_renderer] = create_mock<MockMapRenderer>();
    EXPECT_CALL(map_renderer, set_colours).Times(1);

    auto window = register_test_module().with_map_renderer_source([&](auto&&) { return std::move(map_renderer_ptr); }).build();
    window->set_settings({});
}

TEST(ViewerUI, OnMinimapColoursEventRaised)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto window = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    auto raised = false;
    auto token = window->on_settings += [&](auto&&)
    {
        raised = true;
    };

    settings_window.on_minimap_colours({});
    ASSERT_TRUE(raised);
}

TEST(ViewerUI, OnDefaultRouteColourEventRaised)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto window = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    std::optional<UserSettings> raised;
    auto token = window->on_settings += [&](auto&& settings)
    {
        raised = settings;
    };

    settings_window.on_default_route_colour(Colour::Yellow);

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value().route_colour, Colour::Yellow);
}

TEST(ViewerUI, OnDefaultWaypointColourRaised)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto window = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    std::optional<UserSettings> raised;
    auto token = window->on_settings += [&](auto&& settings)
    {
        raised = settings;
    };

    settings_window.on_default_waypoint_colour(Colour::Yellow);

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value().waypoint_colour, Colour::Yellow);
}

TEST(ViewerUI, SetDefaultRouteColourCalled)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto window = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();
    EXPECT_CALL(settings_window, set_default_route_colour(Colour::Yellow)).Times(1);

    UserSettings settings;
    settings.route_colour = Colour::Yellow;
    window->set_settings(settings);
}

TEST(ViewerUI, SetDefaultWaypointColourCalled)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto window = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();
    EXPECT_CALL(settings_window, set_default_waypoint_colour(Colour::Yellow)).Times(1);

    UserSettings settings;
    settings.waypoint_colour = Colour::Yellow;
    window->set_settings(settings);
}

TEST(ViewerUI, OnCopyEventForwarded)
{
    auto [context_menu_ptr, context_menu] = create_mock<MockContextMenu>();
    auto ui = register_test_module().with_context_menu(std::move(context_menu_ptr)).build();

    std::optional<trview::IContextMenu::CopyType> raised;
    auto token = ui->on_copy += [&](auto&& value)
    {
        raised = value;
    };

    context_menu.on_copy(trview::IContextMenu::CopyType::Position);

    ASSERT_TRUE(raised);
    ASSERT_EQ(raised, trview::IContextMenu::CopyType::Position);
}

TEST(ViewerUI, OnRouteStartupEventRaised)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto ui = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    std::optional<UserSettings> settings;
    auto token = ui->on_settings += [&](auto raised)
    {
        settings = raised;
    };

    settings_window.on_route_startup(true);

    ASSERT_TRUE(settings);
    ASSERT_TRUE(settings.value().route_startup);
}

TEST(ViewerUI, SetRouteStartupUpdatesSettingsWindow)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    EXPECT_CALL(settings_window, set_route_startup(true)).Times(1);

    auto ui = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    UserSettings settings{};
    settings.route_startup = true;
    ui->set_settings(settings);
}

TEST(ViewerUI, OnTriggerSelectedEventForwarded)
{
    auto [context_menu_ptr, context_menu] = create_mock<MockContextMenu>();
    auto ui = register_test_module().with_context_menu(std::move(context_menu_ptr)).build();

    std::optional<std::weak_ptr<ITrigger>> raised;
    auto token = ui->on_select_trigger += [&](auto trigger)
    {
        raised = trigger;
    };

    auto expected = mock_shared<MockTrigger>();
    context_menu.on_trigger_selected(expected);


    ASSERT_TRUE(raised);
    auto raised_ptr = raised.value().lock();
    ASSERT_TRUE(raised_ptr);
    ASSERT_EQ(raised_ptr.get(), expected.get());
}

TEST(ViewerUI, SetTriggeredByUpdatesContextMenu)
{
    auto [context_menu_ptr, context_menu] = create_mock<MockContextMenu>();
    auto ui = register_test_module().with_context_menu(std::move(context_menu_ptr)).build();
    std::vector<std::weak_ptr<ITrigger>> actual;
    EXPECT_CALL(context_menu, set_triggered_by).Times(1);

    ui->set_triggered_by({ mock_shared<MockTrigger>() });
}


TEST(ViewerUI, FovEventRaised)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto ui = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    std::optional<UserSettings> settings;
    auto token = ui->on_settings += [&](auto raised)
    {
        settings = raised;
    };

    settings_window.on_camera_fov(1.0f);

    ASSERT_TRUE(settings);
    ASSERT_EQ(settings.value().fov, 1.0f);
}

TEST(ViewerUI, SetFovUpdatesSettingsWindow)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    EXPECT_CALL(settings_window, set_fov(1.0f)).Times(1);

    auto ui = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    UserSettings settings{};
    settings.fov = 1.0f;
    ui->set_settings(settings);
}

TEST(ViewerUI, OnCameraSinkStartupEventRaised)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto ui = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    std::optional<UserSettings> settings;
    auto token = ui->on_settings += [&](auto raised)
    {
        settings = raised;
    };

    settings_window.on_camera_sink_startup(true);

    ASSERT_TRUE(settings);
    ASSERT_TRUE(settings.value().camera_sink_startup);
}

TEST(ViewerUI, SetCameraSinkStartupUpdatesSettingsWindow)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    EXPECT_CALL(settings_window, set_camera_sink_startup(true)).Times(1);

    auto ui = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    UserSettings settings{};
    settings.camera_sink_startup = true;
    ui->set_settings(settings);
}

TEST(ViewerUI, OnPluginDirectoriesEventRaised)
{
    const std::vector<std::string> expected{ "test " };
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto ui = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    std::optional<UserSettings> settings;
    auto token = ui->on_settings += [&](auto raised)
    {
        settings = raised;
    };

    settings_window.on_plugin_directories(expected);

    ASSERT_TRUE(settings);
    ASSERT_EQ(settings.value().plugin_directories, expected);
}

TEST(ViewerUI, SetPluginDirectoriesUpdatesSettingsWindow)
{
    const std::vector<std::string> expected{ "test " };
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    EXPECT_CALL(settings_window, set_plugin_directories(expected)).Times(1);

    auto ui = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    UserSettings settings{};
    settings.plugin_directories = expected;
    ui->set_settings(settings);
}


TEST(ViewerUI, FontForwarded)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto ui = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    std::optional<std::pair<std::string, FontSetting>> raised;
    auto token = ui->on_font += [&](auto&& key, auto&& value) { raised = { key, value }; };

    settings_window.on_font("test", { .name = "Test", .filename = "test.ttf", .size = 100 });

    ASSERT_EQ(raised.has_value(), true);
    ASSERT_EQ(raised->first, "test");
    ASSERT_EQ(raised->second.name, "Test");
    ASSERT_EQ(raised->second.filename, "test.ttf");
    ASSERT_EQ(raised->second.size, 100);
}

TEST(ViewerUI, OnStaticsStartupEventRaised)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto ui = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    std::optional<UserSettings> settings;
    auto token = ui->on_settings += [&](auto raised)
        {
            settings = raised;
        };

    settings_window.on_statics_startup(true);

    ASSERT_TRUE(settings);
    ASSERT_TRUE(settings.value().statics_startup);
}

TEST(ViewerUI, SetStaticsStartupUpdatesSettingsWindow)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    EXPECT_CALL(settings_window, set_statics_startup(true)).Times(1);

    auto ui = register_test_module().with_settings_window(std::move(settings_window_ptr)).build();

    UserSettings settings{};
    settings.statics_startup = true;
    ui->set_settings(settings);
}

TEST(ViewerUI, NgPlusEnabled)
{
    auto [view_options_ptr, view_options] = create_mock<MockViewOptions>();
    EXPECT_CALL(view_options, set_ng_plus_enabled(true)).Times(1);

    auto ui = register_test_module().with_view_options(std::move(view_options_ptr)).build();

    auto item = mock_shared<MockItem>()->with_ng_plus(false);
    std::vector<std::weak_ptr<IItem>> items{ item };
    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, items).WillByDefault(testing::Return(items));

    ui->set_level(level);
}

TEST(ViewerUI, NgPlusDisabled)
{
    auto [view_options_ptr, view_options] = create_mock<MockViewOptions>();
    EXPECT_CALL(view_options, set_ng_plus_enabled(false)).Times(1);

    auto ui = register_test_module().with_view_options(std::move(view_options_ptr)).build();

    auto item = mock_shared<MockItem>()->with_ng_plus(std::nullopt);
    std::vector<std::weak_ptr<IItem>> items{ item };
    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, items).WillByDefault(testing::Return(items));

    ui->set_level(level);
}

