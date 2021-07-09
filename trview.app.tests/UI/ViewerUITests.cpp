#include <trview.app/UI/ViewerUI.h>
#include <trview.app/Mocks/Graphics/ITextureStorage.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.ui.render/Mocks/IRenderer.h>
#include <trview.ui.render/Mocks/IMapRenderer.h>
#include <trview.ui/Mocks/Input/IInput.h>
#include <trview.tests.common/Window.h>
#include <trview.app/Mocks/UI/ISettingsWindow.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace trview::ui;
using namespace trview::ui::mocks;
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
            IInput::Source input_source{ [](auto&&...) { return std::make_unique<MockInput>(); } };
            IRenderer::Source ui_renderer_source{ [](auto&&...) { return std::make_unique<MockRenderer>(); }};
            IMapRenderer::Source map_renderer_source{ [](auto&&...) { return std::make_unique<MockMapRenderer>(); }};
            ISettingsWindow::Source settings_window_source{ [](auto&&...) { return std::make_unique<MockSettingsWindow>(); }};

            std::unique_ptr<ViewerUI> build()
            {
                EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
                return std::make_unique<ViewerUI>(window, texture_storage, shortcuts, std::move(input_source),
                    ui_renderer_source, map_renderer_source, settings_window_source);
            }

            test_module& with_settings_window_source(const ISettingsWindow::Source& source)
            {
                settings_window_source = source;
                return *this;
            }
        };
        return test_module{};
    }
}

TEST(ViewerUI, OnCameraDisplayDegreesEventRaised)
{
    auto [settings_window_ptr, settings_window] = create_mock<MockSettingsWindow>();
    auto settings_window_ptr_actual = std::move(settings_window_ptr);
    auto ui = register_test_module().with_settings_window_source([&](auto&&...) { return std::move(settings_window_ptr_actual); }).build();

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