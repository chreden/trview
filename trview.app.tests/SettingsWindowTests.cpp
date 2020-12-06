#include <trview.app/UI/SettingsWindow.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/Button.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Slider.h>

using namespace trview;
using namespace trview::ui;

TEST(SettingsWindow, SetVSyncUpdatesCheckbox)
{
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto checkbox = host.find<Checkbox>("VSync");
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    std::optional<bool> received_value;
    auto token = window.on_vsync += [&](bool value)
    {
        received_value = value;
    };

    window.set_vsync(true);
    ASSERT_TRUE(checkbox->state());
    ASSERT_FALSE(received_value.has_value());
}

TEST(SettingsWindow, ClickingVSyncRaisesEvent)
{
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    std::optional<bool> received_value;
    auto token = window.on_vsync += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>("VSync");
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetGoToLaraUpdatesCheckbox)
{
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto checkbox = host.find<Checkbox>("GoToLara");
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    std::optional<bool> received_value;
    auto token = window.on_go_to_lara += [&](bool value)
    {
        received_value = value;
    };

    window.set_go_to_lara(true);
    ASSERT_TRUE(checkbox->state());
    ASSERT_FALSE(received_value.has_value());
}

TEST(SettingsWindow, ClickingGoToLaraRaisesEvent)
{
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    std::optional<bool> received_value;
    auto token = window.on_go_to_lara += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>("GoToLara");
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetInvertMapControlsUpdatesCheckbox)
{
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto checkbox = host.find<Checkbox>("InvertMapControls");
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    std::optional<bool> received_value;
    auto token = window.on_invert_map_controls += [&](bool value)
    {
        received_value = value;
    };

    window.set_invert_map_controls(true);
    ASSERT_TRUE(checkbox->state());
    ASSERT_FALSE(received_value.has_value());
}

TEST(SettingsWindow, ClickingInvertMapControlsRaisesEvent)
{
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    std::optional<bool> received_value;
    auto token = window.on_invert_map_controls += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>("InvertMapControls");
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetItemsWindowOnStartupUpdatesCheckbox)
{
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto checkbox = host.find<Checkbox>("ItemsStartup");
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
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    std::optional<bool> received_value;
    auto token = window.on_items_startup += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>("ItemsStartup");
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetTriggersWindowOnStartupUpdatesCheckbox)
{
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto checkbox = host.find<Checkbox>("TriggersStartup");
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
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    std::optional<bool> received_value;
    auto token = window.on_triggers_startup += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>("TriggersStartup");
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetRoomsWindowOnStartupUpdatesCheckbox)
{
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto checkbox = host.find<Checkbox>("RoomsStartup");
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
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    std::optional<bool> received_value;
    auto token = window.on_rooms_startup += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>("RoomsStartup");
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetOrbitUpdatesCheckbox)
{
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto checkbox = host.find<Checkbox>("AutoOrbit");
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
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    std::optional<bool> received_value;
    auto token = window.on_auto_orbit += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>("AutoOrbit");
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetInvertVerticalPanUpdatesCheckbox)
{
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto checkbox = host.find<Checkbox>("InvertVerticalPan");
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
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    std::optional<bool> received_value;
    auto token = window.on_invert_vertical_pan += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>("InvertVerticalPan");
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetMovementSpeedUpdatesSlider)
{
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto slider = host.find<Slider>("MovementSpeed");
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
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto slider = host.find<Slider>("MovementSpeed");
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
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto slider = host.find<Slider>("Sensitivity");
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
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto slider = host.find<Slider>("Sensitivity");
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
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto checkbox = host.find<Checkbox>("Acceleration");
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
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    std::optional<bool> received_value;
    auto token = window.on_camera_acceleration += [&](bool value)
    {
        received_value = value;
    };

    auto checkbox = host.find<Checkbox>("Acceleration");
    ASSERT_NE(checkbox, nullptr);
    ASSERT_FALSE(checkbox->state());

    checkbox->clicked(Point());
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetAccelerationRateUpdatesSlider)
{
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto slider = host.find<Slider>("AccelerationRate");
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
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto slider = host.find<Slider>("AccelerationRate");
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
    StackPanel host(Size(), Colour::Transparent);
    SettingsWindow window(host);

    auto control = host.find<Control>("SettingsWindow");
    ASSERT_NE(control, nullptr);
    ASSERT_FALSE(control->visible());

    window.toggle_visibility();
    ASSERT_TRUE(control->visible());

    auto close = control->find<Button>("Close");
    ASSERT_NE(close, nullptr);

    close->clicked(Point());
    ASSERT_FALSE(control->visible());
}

TEST(SettingsWindow, WindowIsCentred)
{
    auto host_size = Size(3000, 2000);
    ui::Window host(host_size, Colour::Transparent);
    SettingsWindow window(host);

    auto control = host.find<Control>("SettingsWindow");
    ASSERT_NE(control, nullptr);

    auto size = control->size();
    auto position = control->position();

    auto calculated_size = host_size / 2 - size / 2;
    ASSERT_EQ(position, Point(calculated_size.width, calculated_size.height));
}
