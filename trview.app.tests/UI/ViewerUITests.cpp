#include <trview.app/UI/ViewerUI.h>
#include <trview.app/Windows/IViewer.h>
#include <trview.app/Mocks/Graphics/ITextureStorage.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.ui.render/Mocks/IMapRenderer.h>
#include <trview.tests.common/Window.h>
#include <trview.app/Mocks/UI/ISettingsWindow.h>
#include <trview.app/Mocks/UI/IViewOptions.h>
#include <trview.app/Mocks/UI/IContextMenu.h>
#include <trview.app/Mocks/UI/ICameraControls.h>
#include <trview.common/Mocks/Windows/IShell.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace trview::ui::render;
using namespace trview::ui::render::mocks;

namespace
{
    Event<> shortcut_handler;

    auto register_test_module()
    {
        struct test_module
        {
            trview::Window window{ create_test_window(L"ViewerUITests") };
            std::shared_ptr<ITextureStorage> texture_storage{ std::make_shared<MockTextureStorage>() };
            std::shared_ptr<MockShortcuts> shortcuts{ std::make_shared<MockShortcuts>() };
            IMapRenderer::Source map_renderer_source{ [](auto&&...) { return std::make_unique<MockMapRenderer>(); }};
            std::unique_ptr<ISettingsWindow> settings_window{ std::make_unique<MockSettingsWindow>() };
            std::unique_ptr<IViewOptions> view_options{ std::make_unique<MockViewOptions>() };
            std::unique_ptr<trview::IContextMenu> context_menu{ std::make_unique<MockContextMenu>() };
            std::unique_ptr<ICameraControls> camera_controls{ std::make_unique<MockCameraControls>() };

            std::unique_ptr<ViewerUI> build()
            {
                EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
                return std::make_unique<ViewerUI>(window, texture_storage, shortcuts, map_renderer_source, std::move(settings_window), std::move(view_options), std::move(context_menu), std::move(camera_controls));
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

    std::optional<CameraMode> raised;
    auto token = ui->on_camera_mode += [&](auto mode)
    {
        raised = mode;
    };

    camera_controls.on_mode_selected(CameraMode::Free);
    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), CameraMode::Free);
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
    EXPECT_CALL(camera_controls, set_mode(CameraMode::Orbit)).Times(1);

    ui->set_camera_mode(CameraMode::Orbit);
}

TEST(ViewerUI, SetCameraProjectionModeUpdatesCameraControls)
{
    auto [camera_controls_ptr, camera_controls] = create_mock<MockCameraControls>();
    auto ui = register_test_module().with_camera_controls(std::move(camera_controls_ptr)).build();
    EXPECT_CALL(camera_controls, set_projection_mode(ProjectionMode::Orthographic)).Times(1);

    ui->set_camera_projection_mode(ProjectionMode::Orthographic);
}
