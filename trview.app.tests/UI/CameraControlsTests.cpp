#include <trview.app/UI/CameraControls.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;

TEST(CameraControls, ResetRaisesEvent)
{
    CameraControls controls;
    TestImgui imgui([&]() { controls.render(); });

    bool raised = false;
    auto token = controls.on_reset += [&]()
    {
        raised = true;
    };

    imgui.click_element("Camera Controls", { CameraControls::Names::reset });

    ASSERT_TRUE(raised);
}

TEST(CameraControls, OrbitRaisesModeSelected)
{
    CameraControls controls;
    TestImgui imgui([&]() { controls.render(); });

    std::optional<CameraMode> selected_mode;
    auto token = controls.on_mode_selected += [&](auto mode)
    {
        selected_mode = mode;
    };

    imgui.click_element("Camera Controls", { CameraControls::Names::mode });
    imgui.click_element("##Combo_00", { "Orbit" });

    ASSERT_TRUE(selected_mode.has_value());
    ASSERT_EQ(selected_mode.value(), CameraMode::Orbit);
}

TEST(CameraControls, FreeRaisesModeSelected)
{
    CameraControls controls;
    TestImgui imgui([&]() { controls.render(); });

    std::optional<CameraMode> selected_mode;
    auto token = controls.on_mode_selected += [&](auto mode)
    {
        selected_mode = mode;
    };

    imgui.click_element("Camera Controls", { CameraControls::Names::mode });
    imgui.click_element("##Combo_00", { "Free" });

    ASSERT_TRUE(selected_mode.has_value());
    ASSERT_EQ(selected_mode.value(), CameraMode::Free);
}

TEST(CameraControls, AxisRaisesModeSelected)
{
    CameraControls controls;
    TestImgui imgui([&]() { controls.render(); });

    std::optional<CameraMode> selected_mode;
    auto token = controls.on_mode_selected += [&](auto mode)
    {
        selected_mode = mode;
    };

    imgui.click_element("Camera Controls", { CameraControls::Names::mode });
    imgui.click_element("##Combo_00", { "Axis" });

    ASSERT_TRUE(selected_mode.has_value());
    ASSERT_EQ(selected_mode.value(), CameraMode::Axis);
}

TEST(CameraControls, OrthoRaisesProjectionMode)
{
    CameraControls controls;
    TestImgui imgui([&]() { controls.render(); });

    std::optional<ProjectionMode> selected_mode;
    auto token = controls.on_projection_mode_selected += [&](auto mode)
    {
        selected_mode = mode;
    };

    imgui.click_element("Camera Controls", { CameraControls::Names::projection_mode });
    imgui.click_element("##Combo_00", { "Orthographic" });

    ASSERT_TRUE(selected_mode.has_value());
    ASSERT_EQ(selected_mode.value(), ProjectionMode::Orthographic);
}

TEST(CameraControls, PrespectiveRaisesProjectionMode)
{
    CameraControls controls;
    TestImgui imgui([&]() { controls.render(); });

    std::optional<ProjectionMode> selected_mode;
    auto token = controls.on_projection_mode_selected += [&](auto mode)
    {
        selected_mode = mode;
    };

    imgui.click_element("Camera Controls", { CameraControls::Names::projection_mode });
    imgui.click_element("##Combo_00", { "Perspective" });

    ASSERT_TRUE(selected_mode.has_value());
    ASSERT_EQ(selected_mode.value(), ProjectionMode::Perspective);
}

TEST(CameraControls, SetModeUpdatesCombo)
{
    CameraControls controls;
    TestImgui imgui([&]() { controls.render(); });

    imgui.render();
    ASSERT_EQ(imgui.item_text("Camera Controls", { CameraControls::Names::mode }), "Orbit");

    controls.set_mode(CameraMode::Axis);
    imgui.render();
    ASSERT_EQ(imgui.item_text("Camera Controls", { CameraControls::Names::mode }), "Axis");
}


TEST(CameraControls, SetProjectionModeUpdatesCombo)
{
    CameraControls controls;
    TestImgui imgui([&]() { controls.render(); });

    imgui.render();
    ASSERT_EQ(imgui.item_text("Camera Controls", { CameraControls::Names::projection_mode }), "Perspective");

    controls.set_projection_mode(ProjectionMode::Orthographic);
    imgui.render();
    ASSERT_EQ(imgui.item_text("Camera Controls", { CameraControls::Names::projection_mode }), "Orthographic");
}
