#include <trview.app/UI/SettingsWindow.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;

TEST(SettingsWindow, SetVSyncUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags("Settings", { "TabBar", "General", SettingsWindow::Names::vsync }) & ImGuiItemStatusFlags_Checked);

    window.set_vsync(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags("Settings", { "TabBar", "General", SettingsWindow::Names::vsync }) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingVSyncRaisesEvent)
{
    SettingsWindow window;
    window.toggle_visibility();

    std::optional<bool> received_value;
    auto token = window.on_vsync += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags("Settings", { "TabBar", "General", SettingsWindow::Names::vsync }) & ImGuiItemStatusFlags_Checked);
    imgui.click_element("Settings", { "TabBar", "General", SettingsWindow::Names::vsync });
    ASSERT_TRUE(imgui.status_flags("Settings", { "TabBar", "General", SettingsWindow::Names::vsync }) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetGoToLaraUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags("Settings", { "TabBar", "General", SettingsWindow::Names::go_to_lara }) & ImGuiItemStatusFlags_Checked);

    window.set_go_to_lara(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags("Settings", { "TabBar", "General", SettingsWindow::Names::go_to_lara }) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingGoToLaraRaisesEvent)
{
    SettingsWindow window;
    window.toggle_visibility();

    std::optional<bool> received_value;
    auto token = window.on_go_to_lara += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags("Settings", { "TabBar", "General", SettingsWindow::Names::go_to_lara }) & ImGuiItemStatusFlags_Checked);
    imgui.click_element("Settings", { "TabBar", "General", SettingsWindow::Names::go_to_lara });
    ASSERT_TRUE(imgui.status_flags("Settings", { "TabBar", "General", SettingsWindow::Names::go_to_lara }) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetInvertMapControlsUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags("Settings", { "TabBar", "General", SettingsWindow::Names::invert_map_controls }) & ImGuiItemStatusFlags_Checked);

    window.set_invert_map_controls(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags("Settings", { "TabBar", "General", SettingsWindow::Names::invert_map_controls }) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingInvertMapControlsRaisesEvent)
{
    SettingsWindow window;
    window.toggle_visibility();

    std::optional<bool> received_value;
    auto token = window.on_invert_map_controls += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags("Settings", { "TabBar", "General", SettingsWindow::Names::invert_map_controls }) & ImGuiItemStatusFlags_Checked);
    imgui.click_element("Settings", { "TabBar", "General", SettingsWindow::Names::invert_map_controls });
    ASSERT_TRUE(imgui.status_flags("Settings", { "TabBar", "General", SettingsWindow::Names::invert_map_controls }) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

/*
TEST(SettingsWindow, SetItemsWindowOnStartupUpdatesCheckbox)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::items_startup);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    std::optional<bool> received_value;
    auto token = window.on_items_startup += [&](bool value)
    {
        received_value = value;
    };

    window.set_items_startup(true);
    ASSERT_TRUE(checkbox->state());
    ASSERT_FALSE(received_value.has_value());
}

TEST(SettingsWindow, ClickingItemsWindowOnStartupRaisesEvent)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<bool> received_value;
    auto token = window.on_items_startup += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::items_startup);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetTriggersWindowOnStartupUpdatesCheckbox)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::triggers_startup);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    std::optional<bool> received_value;
    auto token = window.on_triggers_startup += [&](bool value)
    {
        received_value = value;
    };

    window.set_triggers_startup(true);
    ASSERT_TRUE(checkbox->state());
    ASSERT_FALSE(received_value.has_value());
}

TEST(SettingsWindow, ClickingTriggersWindowOnStartupRaisesEvent)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<bool> received_value;
    auto token = window.on_triggers_startup += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::triggers_startup);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetRoomsWindowOnStartupUpdatesCheckbox)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::rooms_startup);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    std::optional<bool> received_value;
    auto token = window.on_rooms_startup += [&](bool value)
    {
        received_value = value;
    };

    window.set_rooms_startup(true);
    ASSERT_TRUE(checkbox->state());
    ASSERT_FALSE(received_value.has_value());
}

TEST(SettingsWindow, ClickingRoomsWindowOnStartupRaisesEvent)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<bool> received_value;
    auto token = window.on_rooms_startup += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::rooms_startup);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetOrbitUpdatesCheckbox)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::auto_orbit);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    std::optional<bool> received_value;
    auto token = window.on_auto_orbit += [&](bool value)
    {
        received_value = value;
    };

    window.set_auto_orbit(true);
    ASSERT_TRUE(checkbox->state());
    ASSERT_FALSE(received_value.has_value());
}

TEST(SettingsWindow, ClickingOrbitRaisesEvent)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<bool> received_value;
    auto token = window.on_auto_orbit += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::auto_orbit);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetInvertVerticalPanUpdatesCheckbox)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::invert_vertical_pan);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    std::optional<bool> received_value;
    auto token = window.on_invert_vertical_pan += [&](bool value)
    {
        received_value = value;
    };

    window.set_invert_vertical_pan(true);
    ASSERT_TRUE(checkbox->state());
    ASSERT_FALSE(received_value.has_value());
}

TEST(SettingsWindow, ClickingInvertVerticalPanRaisesEvent)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<bool> received_value;
    auto token = window.on_invert_vertical_pan += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::invert_vertical_pan);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetMovementSpeedUpdatesSlider)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto slider = host.find<Slider>(SettingsWindow::Names::movement_speed);
    ASSERT_NE(slider, nullptr);
    ASSERT_EQ(slider->value(), 0.0f);

    std::optional<bool> received_value;
    auto token = window.on_movement_speed_changed += [&](bool value)
    {
        received_value = value;
    };

    window.set_movement_speed(0.5f);
    ASSERT_EQ(slider->value(), 0.5f);
    ASSERT_FALSE(received_value.has_value());
}


TEST(SettingsWindow, ClickingMovementSpeedRaisesEvent)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto slider = host.find<Slider>(SettingsWindow::Names::movement_speed);
    ASSERT_NE(slider, nullptr);
    ASSERT_EQ(slider->value(), 0.0f);

    std::optional<float> received_value;
    auto token = window.on_movement_speed_changed += [&](float value)
    {
        received_value = value;
    };

    slider->clicked(Point(slider->size().width / 2, 0));
    ASSERT_TRUE(received_value.has_value());
    ASSERT_EQ(received_value.value(), 0.5f);
}

TEST(SettingsWindow, SetSensitivitySlider)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto slider = host.find<Slider>(SettingsWindow::Names::sensitivity);
    ASSERT_NE(slider, nullptr);
    ASSERT_EQ(slider->value(), 0.0f);

    std::optional<bool> received_value;
    auto token = window.on_sensitivity_changed += [&](bool value)
    {
        received_value = value;
    };

    window.set_sensitivity(0.5f);
    ASSERT_EQ(slider->value(), 0.5f);
    ASSERT_FALSE(received_value.has_value());
}


TEST(SettingsWindow, ClickingSensitivityRaisesEvent)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto slider = host.find<Slider>(SettingsWindow::Names::sensitivity);
    ASSERT_NE(slider, nullptr);
    ASSERT_EQ(slider->value(), 0.0f);

    std::optional<float> received_value;
    auto token = window.on_sensitivity_changed += [&](float value)
    {
        received_value = value;
    };

    slider->clicked(Point(slider->size().width / 2, 0));
    ASSERT_TRUE(received_value.has_value());
    ASSERT_EQ(received_value.value(), 0.5f);
}

TEST(SettingsWindow, SetAccelerationUpdatesCheckbox)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::acceleration);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    std::optional<bool> received_value;
    auto token = window.on_camera_acceleration += [&](bool value)
    {
        received_value = value;
    };

    window.set_camera_acceleration(true);
    ASSERT_TRUE(checkbox->state());
    ASSERT_FALSE(received_value.has_value());
}

TEST(SettingsWindow, ClickingAccelerationRaisesEvent)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<bool> received_value;
    auto token = window.on_camera_acceleration += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::acceleration);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetAccelerationRateUpdatesSlider)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto slider = host.find<Slider>(SettingsWindow::Names::acceleration_rate);
    ASSERT_NE(slider, nullptr);
    ASSERT_EQ(slider->value(), 0.0f);

    std::optional<bool> received_value;
    auto token = window.on_camera_acceleration_rate += [&](bool value)
    {
        received_value = value;
    };

    window.set_camera_acceleration_rate(0.5f);
    ASSERT_EQ(slider->value(), 0.5f);
    ASSERT_FALSE(received_value.has_value());
}


TEST(SettingsWindow, ClickingAccelerationRateRaisesEvent)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto slider = host.find<Slider>(SettingsWindow::Names::acceleration_rate);
    ASSERT_NE(slider, nullptr);
    ASSERT_EQ(slider->value(), 0.0f);

    std::optional<float> received_value;
    auto token = window.on_camera_acceleration_rate += [&](float value)
    {
        received_value = value;
    };

    slider->clicked(Point(slider->size().width / 2, 0));
    ASSERT_TRUE(received_value.has_value());
    ASSERT_EQ(received_value.value(), 0.5f);
}

TEST(SettingsWindow, CloseClosesWindow)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto control = host.find<Control>("SettingsWindow");
    ASSERT_NE(control, nullptr);
    ASSERT_FALSE(control->visible());

    window.toggle_visibility();
    ASSERT_TRUE(control->visible());

    auto close = control->find<Button>(SettingsWindow::Names::close);
    ASSERT_NE(close, nullptr);

    close->clicked(Point());
    ASSERT_FALSE(control->visible());
}

TEST(SettingsWindow, WindowIsCentred)
{
    auto host_size = Size(3000, 2000);
    ui::Window host(host_size, Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto control = host.find<Control>("SettingsWindow");
    ASSERT_NE(control, nullptr);

    auto size = control->size();
    auto position = control->position();

    auto calculated_size = host_size / 2 - size / 2;
    ASSERT_EQ(position, Point(calculated_size.width, calculated_size.height));
}

TEST(SettingsWindow, ClickingCameraDegreesRaisesEvent)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<bool> received_value;
    auto token = window.on_camera_display_degrees += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::camera_display_degrees);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_TRUE(received_value.has_value());
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetCameraDegreesUpdatesCheckbox)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::camera_display_degrees);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    std::optional<bool> received_value;
    auto token = window.on_camera_display_degrees += [&](bool value)
    {
        received_value = value;
    };

    window.set_camera_display_degrees(true);
    ASSERT_TRUE(checkbox->state());
    ASSERT_FALSE(received_value.has_value());
}


TEST(SettingsWindow, ClickingRandomizerToolsRaisesEvent)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<bool> received_value;
    auto token = window.on_randomizer_tools += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::randomizer_tools);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_TRUE(received_value.has_value());
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetRandomizerToolsUpdatesCheckbox)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = host.find<Checkbox>(SettingsWindow::Names::randomizer_tools);
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    std::optional<bool> received_value;
    auto token = window.on_randomizer_tools += [&](bool value)
    {
        received_value = value;
    };

    window.set_randomizer_tools(true);
    ASSERT_TRUE(checkbox->state());
    ASSERT_FALSE(received_value.has_value());
}

TEST(SettingsWindow, ChangingMaxRecentFilesRaisesEvent)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<uint32_t> received_value;
    auto token = window.on_max_recent_files += [&](uint32_t value)
    {
        received_value = value;
    };

    auto numeric_up_down = host.find<NumericUpDown>(SettingsWindow::Names::max_recent_files);
    ASSERT_NE(numeric_up_down, nullptr);
    ASSERT_EQ(numeric_up_down->value(), 0);

    // Click on the up button.
    auto up = numeric_up_down->find<Button>(NumericUpDown::Names::up);
    ASSERT_NE(up, nullptr);
    up->clicked(Point());

    ASSERT_TRUE(received_value.has_value());
    ASSERT_EQ(received_value.value(), 1);
}

TEST(SettingsWindow, SetMaxRecentFilesUpdatesNumericUpDown)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto numeric_up_down = host.find<NumericUpDown>(SettingsWindow::Names::max_recent_files);
    ASSERT_NE(numeric_up_down, nullptr);
    ASSERT_EQ(numeric_up_down->value(), 0);

    std::optional<uint32_t> received_value;
    auto token = window.on_max_recent_files += [&](uint32_t value)
    {
        received_value = value;
    };

    window.set_max_recent_files(5);
    ASSERT_EQ(numeric_up_down->value(), 5);
    ASSERT_FALSE(received_value.has_value());
}
*/