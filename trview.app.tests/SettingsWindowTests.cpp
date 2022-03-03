#include <trview.app/UI/SettingsWindow.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;

TEST(SettingsWindow, SetVSyncUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::vsync)) & ImGuiItemStatusFlags_Checked);

    window.set_vsync(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::vsync)) & ImGuiItemStatusFlags_Checked);
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
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::vsync)) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::vsync));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::vsync)) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetGoToLaraUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::go_to_lara)) & ImGuiItemStatusFlags_Checked);

    window.set_go_to_lara(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::go_to_lara)) & ImGuiItemStatusFlags_Checked);
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
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::go_to_lara)) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::go_to_lara));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::go_to_lara)) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetInvertMapControlsUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Minimap"));
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Minimap").id(SettingsWindow::Names::invert_map_controls)) & ImGuiItemStatusFlags_Checked);

    window.set_invert_map_controls(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Minimap").id(SettingsWindow::Names::invert_map_controls)) & ImGuiItemStatusFlags_Checked);
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
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Minimap"));
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Minimap").id(SettingsWindow::Names::invert_map_controls)) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("Minimap").id(SettingsWindow::Names::invert_map_controls));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Minimap").id(SettingsWindow::Names::invert_map_controls)) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}


TEST(SettingsWindow, SetItemsWindowOnStartupUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::items_startup)) & ImGuiItemStatusFlags_Checked);

    window.set_items_startup(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::items_startup)) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingItemsWindowOnStartupRaisesEvent)
{
    SettingsWindow window;
    window.toggle_visibility();

    std::optional<bool> received_value;
    auto token = window.on_items_startup += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::items_startup)) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::items_startup));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::items_startup)) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetTriggersWindowOnStartupUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::triggers_startup)) & ImGuiItemStatusFlags_Checked);

    window.set_triggers_startup(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::triggers_startup)) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingTriggersWindowOnStartupRaisesEvent)
{
    SettingsWindow window;
    window.toggle_visibility();

    std::optional<bool> received_value;
    auto token = window.on_triggers_startup += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::triggers_startup)) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::triggers_startup));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::triggers_startup)) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetRoomsWindowOnStartupUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::rooms_startup )) & ImGuiItemStatusFlags_Checked);

    window.set_rooms_startup(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::rooms_startup )) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingRoomsWindowOnStartupRaisesEvent)
{
    SettingsWindow window;
    window.toggle_visibility();

    std::optional<bool> received_value;
    auto token = window.on_rooms_startup += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::rooms_startup )) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::rooms_startup ));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::rooms_startup )) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetOrbitUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::auto_orbit )) & ImGuiItemStatusFlags_Checked);

    window.set_auto_orbit(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::auto_orbit )) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingOrbitRaisesEvent)
{
    SettingsWindow window;
    window.toggle_visibility();

    std::optional<bool> received_value;
    auto token = window.on_auto_orbit += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::auto_orbit )) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::auto_orbit ));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::auto_orbit )) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetInvertVerticalPanUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::invert_vertical_pan )) & ImGuiItemStatusFlags_Checked);

    window.set_invert_vertical_pan(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::invert_vertical_pan )) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingInvertVerticalPanRaisesEvent)
{
    SettingsWindow window;
    window.toggle_visibility();

    std::optional<bool> received_value;
    auto token = window.on_invert_vertical_pan += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::invert_vertical_pan )) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::invert_vertical_pan ));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::invert_vertical_pan )) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}
/*
TEST(SettingsWindow, SetMovementSpeedUpdatesSlider)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(mock_shared<MockShell>()));

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
    SettingsWindow window(host, std::make_shared<JsonLoader>(mock_shared<MockShell>()));

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
    SettingsWindow window(host, std::make_shared<JsonLoader>(mock_shared<MockShell>()));

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
    SettingsWindow window(host, std::make_shared<JsonLoader>(mock_shared<MockShell>()));

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
*/
TEST(SettingsWindow, SetAccelerationUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration )) & ImGuiItemStatusFlags_Checked);

    window.set_camera_acceleration(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration )) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingAccelerationRaisesEvent)
{
    SettingsWindow window;
    window.toggle_visibility();

    std::optional<bool> received_value;
    auto token = window.on_camera_acceleration += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration )) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration ));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration )) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}
/*
TEST(SettingsWindow, SetAccelerationRateUpdatesSlider)
{
    ui::Window host(Size(), Colour::Transparent);
    SettingsWindow window(host, std::make_shared<JsonLoader>(mock_shared<MockShell>()));

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
    // ui::Window host(Size(), Colour::Transparent);
    // SettingsWindow window(host, std::make_shared<JsonLoader>(mock_shared<MockShell>()));
    SettingsWindow window;
    window.toggle_visibility();

    std::optional<float> received_value;
    auto token = window.on_camera_acceleration_rate += [&](float value)
    {
        received_value = value;
    };

    // auto slider = host.find<Slider>(SettingsWindow::Names::acceleration_rate);
    // ASSERT_NE(slider, nullptr);
    // ASSERT_EQ(slider->value(), 0.0f);
    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    imgui.click_element(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration_rate });
    // slider->clicked(Point(slider->size().width / 2, 0));
    ASSERT_TRUE(received_value.has_value());
    ASSERT_EQ(received_value.value(), 0.0f);
}
*/
TEST(SettingsWindow, ClickingCameraDegreesRaisesEvent)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::camera_display_degrees )) & ImGuiItemStatusFlags_Checked);

    window.set_camera_display_degrees(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::camera_display_degrees )) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, SetCameraDegreesUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    std::optional<bool> received_value;
    auto token = window.on_camera_display_degrees += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::camera_display_degrees )) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::camera_display_degrees ));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::camera_display_degrees )) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}


TEST(SettingsWindow, ClickingRandomizerToolsRaisesEvent)
{
    SettingsWindow window;
    window.toggle_visibility();

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::randomizer_tools )) & ImGuiItemStatusFlags_Checked);

    window.set_randomizer_tools(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::randomizer_tools )) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, SetRandomizerToolsUpdatesCheckbox)
{
    SettingsWindow window;
    window.toggle_visibility();

    std::optional<bool> received_value;
    auto token = window.on_randomizer_tools += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::randomizer_tools )) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::randomizer_tools ));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::randomizer_tools )) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, ChangingMaxRecentFilesRaisesEvent)
{
    SettingsWindow window;
    window.toggle_visibility();

    std::optional<uint32_t> received_value;
    auto token = window.on_max_recent_files += [&](uint32_t value)
    {
        received_value = value;
    };

    tests::TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").push(SettingsWindow::Names::max_recent_files).id("+"));

    ASSERT_TRUE(received_value.has_value());
    ASSERT_EQ(received_value.value(), 11);
}

TEST(SettingsWindow, SetMaxRecentFilesUpdatesNumericUpDown)
{
    SettingsWindow window;
    window.toggle_visibility();

    tests::TestImgui imgui([&]() { window.render(); });
    ASSERT_EQ(imgui.item_text(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::max_recent_files )), "10");

    window.set_max_recent_files(5);
    imgui.render();
    ASSERT_EQ(imgui.item_text(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::max_recent_files )), "5");
}
