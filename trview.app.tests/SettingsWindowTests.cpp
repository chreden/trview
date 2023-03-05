#include <trview.app/UI/Settingswindow.h>
#include "TestImgui.h"

#include <trview.common/Mocks/Windows/IShell.h>
#include <trview.common/Mocks/Windows/IDialogs.h>

using namespace trview;
using namespace trview::tests;
using namespace testing;
using namespace trview::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IDialogs> dialogs{ mock_shared<MockDialogs>() };
            std::shared_ptr<IShell> shell{ mock_shared<MockShell>() };

            std::unique_ptr<SettingsWindow> build()
            {
                return std::make_unique<SettingsWindow>(dialogs, shell);
            }

            test_module& with_dialogs(const std::shared_ptr<IDialogs>& dialogs)
            {
                this->dialogs = dialogs;
                return *this;
            }

            test_module& with_shell(const std::shared_ptr<IShell>& shell)
            {
                this->shell = shell;
                return *this;
            }
        };

        return test_module{};
    }
}

TEST(SettingsWindow, SetVSyncUpdatesCheckbox)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::vsync)) & ImGuiItemStatusFlags_Checked);

    window->set_vsync(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::vsync)) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingVSyncRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<bool> received_value;
    auto token = window->on_vsync += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::vsync)) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::vsync));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::vsync)) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetGoToLaraUpdatesCheckbox)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::go_to_lara)) & ImGuiItemStatusFlags_Checked);

    window->set_go_to_lara(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::go_to_lara)) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingGoToLaraRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<bool> received_value;
    auto token = window->on_go_to_lara += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::go_to_lara)) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::go_to_lara));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::go_to_lara)) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetInvertMapControlsUpdatesCheckbox)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Minimap"));
    imgui.render();

    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Minimap").id(SettingsWindow::Names::invert_map_controls)) & ImGuiItemStatusFlags_Checked);

    window->set_invert_map_controls(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Minimap").id(SettingsWindow::Names::invert_map_controls)) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingInvertMapControlsRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<bool> received_value;
    auto token = window->on_invert_map_controls += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Minimap"));
    imgui.render();

    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Minimap").id(SettingsWindow::Names::invert_map_controls)) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("Minimap").id(SettingsWindow::Names::invert_map_controls));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Minimap").id(SettingsWindow::Names::invert_map_controls)) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetItemsWindowOnStartupUpdatesCheckbox)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::items_startup)) & ImGuiItemStatusFlags_Checked);

    window->set_items_startup(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::items_startup)) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingItemsWindowOnStartupRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<bool> received_value;
    auto token = window->on_items_startup += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::items_startup)) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::items_startup));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::items_startup)) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetTriggersWindowOnStartupUpdatesCheckbox)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::triggers_startup)) & ImGuiItemStatusFlags_Checked);

    window->set_triggers_startup(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::triggers_startup)) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingTriggersWindowOnStartupRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<bool> received_value;
    auto token = window->on_triggers_startup += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::triggers_startup)) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::triggers_startup));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::triggers_startup)) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetRoomsWindowOnStartupUpdatesCheckbox)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::rooms_startup )) & ImGuiItemStatusFlags_Checked);

    window->set_rooms_startup(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::rooms_startup )) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingRoomsWindowOnStartupRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<bool> received_value;
    auto token = window->on_rooms_startup += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::rooms_startup )) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::rooms_startup ));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::rooms_startup )) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetOrbitUpdatesCheckbox)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::auto_orbit )) & ImGuiItemStatusFlags_Checked);

    window->set_auto_orbit(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::auto_orbit )) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingOrbitRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<bool> received_value;
    auto token = window->on_auto_orbit += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window->render(); });
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
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::invert_vertical_pan )) & ImGuiItemStatusFlags_Checked);

    window->set_invert_vertical_pan(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::invert_vertical_pan )) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingInvertVerticalPanRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<bool> received_value;
    auto token = window->on_invert_vertical_pan += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::invert_vertical_pan )) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::invert_vertical_pan ));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::invert_vertical_pan )) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetMovementSpeedUpdatesSlider)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();

    ASSERT_EQ(imgui.item_text(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::movement_speed)), "1.000");

    std::optional<bool> received_value;
    auto token = window->on_movement_speed_changed += [&](bool value)
    {
        received_value = value;
    };

    window->set_movement_speed(0.5f);
    imgui.render();
    ASSERT_EQ(imgui.item_text(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::movement_speed)), "0.500");
    ASSERT_FALSE(received_value.has_value());
}

TEST(SettingsWindow, ClickingMovementSpeedRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();

    const auto movement_speed_id = imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::movement_speed);

    ASSERT_EQ(imgui.item_text(movement_speed_id), "1.000");

    std::optional<float> received_value;
    auto token = window->on_movement_speed_changed += [&](float value)
    {
        received_value = value;
    };

    imgui.mouse_down_element(movement_speed_id, movement_speed_id);

    ASSERT_EQ(imgui.item_text(movement_speed_id), "0.000");
    ASSERT_TRUE(received_value.has_value());
    ASSERT_EQ(received_value.value(), 0.0f);
}

TEST(SettingsWindow, SetSensitivitySlider)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();

    ASSERT_EQ(imgui.item_text(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::sensitivity)), "1.000");

    std::optional<bool> received_value;
    auto token = window->on_sensitivity_changed += [&](bool value)
    {
        received_value = value;
    };

    window->set_sensitivity(0.5f);
    imgui.render();
    ASSERT_EQ(imgui.item_text(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::sensitivity)), "0.500");
    ASSERT_FALSE(received_value.has_value());
}

TEST(SettingsWindow, ClickingSensitivityRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();

    const auto sensitivity_id = imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::sensitivity);

    ASSERT_EQ(imgui.item_text(sensitivity_id), "1.000");

    std::optional<float> received_value;
    auto token = window->on_sensitivity_changed += [&](float value)
    {
        received_value = value;
    };

    imgui.mouse_down_element(sensitivity_id, sensitivity_id);

    ASSERT_EQ(imgui.item_text(sensitivity_id), "0.000");
    ASSERT_TRUE(received_value.has_value());
    ASSERT_EQ(received_value.value(), 0.0f);
}

TEST(SettingsWindow, SetAccelerationUpdatesCheckbox)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration )) & ImGuiItemStatusFlags_Checked);

    window->set_camera_acceleration(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration )) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingAccelerationRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<bool> received_value;
    auto token = window->on_camera_acceleration += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration )) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration ));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration )) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetAccelerationRateUpdatesSlider)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();

    ASSERT_EQ(imgui.item_text(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration_rate)), "1.000");

    std::optional<bool> received_value;
    auto token = window->on_camera_acceleration_rate += [&](bool value)
    {
        received_value = value;
    };

    window->set_camera_acceleration_rate(0.5f);
    imgui.render();
    ASSERT_EQ(imgui.item_text(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration_rate)), "0.500");
    ASSERT_FALSE(received_value.has_value());
}


TEST(SettingsWindow, ClickingAccelerationRateRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();

    const auto acceleration_rate_id = imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::acceleration_rate);

    ASSERT_EQ(imgui.item_text(acceleration_rate_id), "1.000");

    std::optional<float> received_value;
    auto token = window->on_camera_acceleration_rate += [&](float value)
    {
        received_value = value;
    };

    imgui.mouse_down_element(acceleration_rate_id, acceleration_rate_id);

    ASSERT_EQ(imgui.item_text(acceleration_rate_id), "0.000");
    ASSERT_TRUE(received_value.has_value());
    ASSERT_EQ(received_value.value(), 0.0f);
}

TEST(SettingsWindow, ClickingCameraDegreesRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::camera_display_degrees )) & ImGuiItemStatusFlags_Checked);

    window->set_camera_display_degrees(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::camera_display_degrees )) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, SetCameraDegreesUpdatesCheckbox)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<bool> received_value;
    auto token = window->on_camera_display_degrees += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window->render(); });
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
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::randomizer_tools )) & ImGuiItemStatusFlags_Checked);

    window->set_randomizer_tools(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::randomizer_tools )) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, SetRandomizerToolsUpdatesCheckbox)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<bool> received_value;
    auto token = window->on_randomizer_tools += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::randomizer_tools )) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::randomizer_tools ));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::randomizer_tools )) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, ChangingMaxRecentFilesRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<uint32_t> received_value;
    auto token = window->on_max_recent_files += [&](uint32_t value)
    {
        received_value = value;
    };

    tests::TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").push(SettingsWindow::Names::max_recent_files).id("+"));

    ASSERT_TRUE(received_value.has_value());
    ASSERT_EQ(received_value.value(), 11);
}

TEST(SettingsWindow, SetMaxRecentFilesUpdatesNumericUpDown)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    tests::TestImgui imgui([&]() { window->render(); });
    ASSERT_EQ(imgui.item_text(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::max_recent_files )), "10");

    window->set_max_recent_files(5);
    imgui.render();
    ASSERT_EQ(imgui.item_text(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::max_recent_files )), "5");
}

TEST(SettingsWindow, SetMapColoursUpdatesColours)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    tests::TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Minimap"));
    imgui.render();

    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Minimap").push("Default").id("##X")), HasSubstr("0"));
    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Minimap").push("Default").id("##Y")), HasSubstr("179"));
    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Minimap").push("Default").id("##Z")), HasSubstr("179"));
    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Minimap").push("Default").id("##W")), HasSubstr("255"));

    MapColours colours;
    colours.set_colour(MapColours::Special::Default, Colour(0.25f, 0.5f, 0.75f, 1.0f));
    window->set_map_colours(colours);
    imgui.render();

    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Minimap").push("Default").id("##X")), HasSubstr("128"));
    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Minimap").push("Default").id("##Y")), HasSubstr("191"));
    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Minimap").push("Default").id("##Z")), HasSubstr("255"));
    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Minimap").push("Default").id("##W")), HasSubstr("64"));
}

TEST(SettingsWindow, OnMinimapColoursRaisedOnResetSpecial)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    MapColours colours;
    colours.set_colour(MapColours::Special::Default, Colour::Red);
    window->set_map_colours(colours);

    std::optional<MapColours> map_colours;
    auto token = window->on_minimap_colours += [&](const auto& colours)
    {
        map_colours = colours;
    };

    tests::TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Minimap"));
    imgui.render();

    imgui.click_element(imgui.id("Settings").push("TabBar").push("Minimap").id("Reset##Default"));

    ASSERT_TRUE(map_colours.has_value());
    ASSERT_EQ(map_colours.value().special_colours().size(), 0.0f);
}

TEST(SettingsWindow, OnMinimapColoursRaisedOnResetNormal)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    MapColours colours;
    colours.set_colour(SectorFlag::Death, Colour::Blue);
    window->set_map_colours(colours);

    std::optional<MapColours> map_colours;
    auto token = window->on_minimap_colours += [&](const auto& colours)
    {
        map_colours = colours;
    };

    tests::TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Minimap"));
    imgui.render();

    imgui.click_element(imgui.id("Settings").push("TabBar").push("Minimap").id("Reset##Death"));

    ASSERT_TRUE(map_colours.has_value());
    ASSERT_EQ(map_colours.value().override_colours().size(), 0.0f);
}

TEST(SettingsWindow, SetDefaultRouteColourUpdatesColours)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    window->set_default_route_colour(Colour(0.5f, 0.75f, 1.0f));

    tests::TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Route"));
    imgui.render();

    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Route").push(SettingsWindow::Names::default_route_colour).id("##X")), HasSubstr("128"));
    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Route").push(SettingsWindow::Names::default_route_colour).id("##Y")), HasSubstr("191"));
    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Route").push(SettingsWindow::Names::default_route_colour).id("##Z")), HasSubstr("255"));
}

TEST(SettingsWindow, SetDefaultWaypointColourUpdatesColours)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    window->set_default_waypoint_colour(Colour(0.5f, 0.75f, 1.0f));

    tests::TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Route"));
    imgui.render();

    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Route").push(SettingsWindow::Names::default_waypoint_colour).id("##X")), HasSubstr("128"));
    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Route").push(SettingsWindow::Names::default_waypoint_colour).id("##Y")), HasSubstr("191"));
    ASSERT_THAT(imgui.item_text(imgui.id("Settings").push("TabBar").push("Route").push(SettingsWindow::Names::default_waypoint_colour).id("##Z")), HasSubstr("255"));
}

TEST(SettingsWindow, SetRouteWindowOnStartupUpdatesCheckbox)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::route_startup)) & ImGuiItemStatusFlags_Checked);

    window->set_route_startup(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::route_startup)) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingRouteWindowOnStartupRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<bool> received_value;
    auto token = window->on_route_startup += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::route_startup)) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::route_startup));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::route_startup)) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, SetFovUpdatesSlider)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();

    ASSERT_EQ(imgui.item_text(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::fov)), "45.000");

    std::optional<float> received_value;
    auto token = window->on_camera_fov += [&](float value)
    {
        received_value = value;
    };

    window->set_fov(0.5f);
    imgui.render();
    ASSERT_EQ(imgui.item_text(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::fov)), "0.500");
    ASSERT_FALSE(received_value.has_value());
}


TEST(SettingsWindow, ClickingFovRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();

    const auto fov_id = imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::fov);

    ASSERT_EQ(imgui.item_text(fov_id), "45.000");

    std::optional<float> received_value;
    auto token = window->on_camera_fov += [&](float value)
    {
        received_value = value;
    };

    imgui.mouse_down_element(fov_id, fov_id);

    ASSERT_EQ(imgui.item_text(fov_id), "10.000");
    ASSERT_TRUE(received_value.has_value());
    ASSERT_EQ(received_value.value(), 10.0f);
}

TEST(SettingsWindow, ClickingResetFovRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();
    window->set_fov(10.0f);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Camera"));
    imgui.render();

    const auto fov_id = imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::fov);

    ASSERT_EQ(imgui.item_text(fov_id), "10.000");

    std::optional<float> received_value;
    auto token = window->on_camera_fov += [&](float value)
    {
        received_value = value;
    };

    imgui.click_element(imgui.id("Settings").push("TabBar").push("Camera").id(SettingsWindow::Names::reset_fov));

    ASSERT_EQ(imgui.item_text(fov_id), "45.000");
    ASSERT_TRUE(received_value.has_value());
    ASSERT_EQ(received_value.value(), 45.0f);
}

TEST(SettingsWindow, SetCameraSinkWindowOnStartupUpdatesCheckbox)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::camera_sink_startup)) & ImGuiItemStatusFlags_Checked);

    window->set_camera_sink_startup(true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::camera_sink_startup)) & ImGuiItemStatusFlags_Checked);
}

TEST(SettingsWindow, ClickingCameraSinkWindowOnStartupRaisesEvent)
{
    auto window = register_test_module().build();
    window->toggle_visibility();

    std::optional<bool> received_value;
    auto token = window->on_camera_sink_startup += [&](bool value)
    {
        received_value = value;
    };

    TestImgui imgui([&]() { window->render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::camera_sink_startup)) & ImGuiItemStatusFlags_Checked);
    imgui.click_element(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::camera_sink_startup));
    ASSERT_TRUE(imgui.status_flags(imgui.id("Settings").push("TabBar").push("General").id(SettingsWindow::Names::camera_sink_startup)) & ImGuiItemStatusFlags_Checked);
    ASSERT_EQ(received_value.has_value(), true);
    ASSERT_TRUE(received_value.value());
}

TEST(SettingsWindow, OpenPluginDirectories)
{
    auto shell = mock_shared<MockShell>();
    EXPECT_CALL(*shell, open(std::wstring(L"path"))).Times(1);

    auto window = register_test_module().with_shell(shell).build();
    window->toggle_visibility();
    window->set_plugin_directories({ "path" });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Plugins"));
    imgui.click_element(imgui.id("Settings").push("TabBar").push("Plugins").push("Directories").id("Open##0"));
}

TEST(SettingsWindow, DeletePluginDirectories)
{
    auto window = register_test_module().build();
    std::optional<std::vector<std::string>> raised;
    auto token = window->on_plugin_directories += [&](const auto& value)
    {
        raised = value;
    };

    window->toggle_visibility();
    window->set_plugin_directories({ "path" });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Plugins"));
    imgui.click_element(imgui.id("Settings").push("TabBar").push("Plugins").push("Directories").id("Delete##0"));

    const std::vector<std::string> expected;
    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), expected);
}

TEST(SettingsWindow, AddPluginDirectories)
{
    auto dialogs = mock_shared<MockDialogs>();
    ON_CALL(*dialogs, open_folder).WillByDefault(Return("test"));

    auto window = register_test_module().with_dialogs(dialogs).build();

    std::optional<std::vector<std::string>> raised;
    auto token = window->on_plugin_directories += [&](const auto& value)
    {
        raised = value;
    };

    window->toggle_visibility();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Settings").push("TabBar").id("Plugins"));
    imgui.click_element(imgui.id("Settings").push("TabBar").push("Plugins").push("Directories").id("Add"));

    const std::vector<std::string> expected{ "test" };
    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), expected);
}