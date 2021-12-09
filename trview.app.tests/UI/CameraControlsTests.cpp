#include <trview.app/UI/CameraControls.h>
#include <trview.ui/Button.h>
#include <trview.ui/JsonLoader.h>
#include <trview.common/Mocks/Windows/IShell.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::ui;

TEST(CameraControls, ResetRaisesEvent)
{
    ui::Window host(Size(), Colour::Transparent);
    CameraControls controls(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    bool raised = false;
    auto token = controls.on_reset += [&]()
    {
        raised = true;
    };

    auto reset = host.find<Button>(CameraControls::Names::reset);
    ASSERT_NE(reset, nullptr);

    reset->clicked(Point());

    ASSERT_TRUE(raised);
}

TEST(CameraControls, OrbitRaisesModeSelected)
{
    ui::Window host(Size(), Colour::Transparent);
    CameraControls controls(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<CameraMode> selected_mode;
    auto token = controls.on_mode_selected += [&](auto mode)
    {
        selected_mode = mode;
    };

    auto orbit = host.find<Checkbox>(CameraControls::Names::orbit);
    ASSERT_NE(orbit, nullptr);

    orbit->clicked(Point());

    ASSERT_TRUE(selected_mode.has_value());
    ASSERT_EQ(selected_mode.value(), CameraMode::Orbit);
}

TEST(CameraControls, FreeRaisesModeSelected)
{
    ui::Window host(Size(), Colour::Transparent);
    CameraControls controls(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<CameraMode> selected_mode;
    auto token = controls.on_mode_selected += [&](auto mode)
    {
        selected_mode = mode;
    };

    auto free = host.find<Checkbox>(CameraControls::Names::free);
    ASSERT_NE(free, nullptr);

    free->clicked(Point());

    ASSERT_TRUE(selected_mode.has_value());
    ASSERT_EQ(selected_mode.value(), CameraMode::Free);
}

TEST(CameraControls, AxisRaisesModeSelected)
{
    ui::Window host(Size(), Colour::Transparent);
    CameraControls controls(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<CameraMode> selected_mode;
    auto token = controls.on_mode_selected += [&](auto mode)
    {
        selected_mode = mode;
    };

    auto axis = host.find<Checkbox>(CameraControls::Names::axis);
    ASSERT_NE(axis, nullptr);

    axis->clicked(Point());

    ASSERT_TRUE(selected_mode.has_value());
    ASSERT_EQ(selected_mode.value(), CameraMode::Axis);
}

TEST(CameraControls, OrthoOnRaisesProjectionMode)
{
    ui::Window host(Size(), Colour::Transparent);
    CameraControls controls(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<ProjectionMode> selected_mode;
    auto token = controls.on_projection_mode_selected += [&](auto mode)
    {
        selected_mode = mode;
    };

    auto ortho = host.find<Checkbox>(CameraControls::Names::ortho);
    ASSERT_NE(ortho, nullptr);

    ortho->clicked(Point());

    ASSERT_TRUE(selected_mode.has_value());
    ASSERT_EQ(selected_mode.value(), ProjectionMode::Orthographic);
}

TEST(CameraControls, OrthoOffRaisesProjectionMode)
{
    ui::Window host(Size(), Colour::Transparent);
    CameraControls controls(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<ProjectionMode> selected_mode;
    auto token = controls.on_projection_mode_selected += [&](auto mode)
    {
        selected_mode = mode;
    };

    auto ortho = host.find<Checkbox>(CameraControls::Names::ortho);
    ASSERT_NE(ortho, nullptr);
    ortho->set_state(true);

    ortho->clicked(Point());

    ASSERT_TRUE(selected_mode.has_value());
    ASSERT_EQ(selected_mode.value(), ProjectionMode::Perspective);
}

TEST(CameraControls, SetModeFreeUpdatesCheckboxes)
{
    ui::Window host(Size(), Colour::Transparent);
    CameraControls controls(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto free = host.find<Checkbox>(CameraControls::Names::free);
    ASSERT_NE(free, nullptr);
    ASSERT_FALSE(free->state());
    auto orbit = host.find<Checkbox>(CameraControls::Names::orbit);
    ASSERT_NE(orbit, nullptr);
    ASSERT_FALSE(orbit->state());
    auto axis = host.find<Checkbox>(CameraControls::Names::axis);
    ASSERT_NE(axis, nullptr);
    ASSERT_FALSE(axis->state());

    controls.set_mode(CameraMode::Free);

    ASSERT_TRUE(free->state());
    ASSERT_FALSE(orbit->state());
    ASSERT_FALSE(axis->state());
}

TEST(CameraControls, SetModeOrbitpdatesCheckboxes)
{
    ui::Window host(Size(), Colour::Transparent);
    CameraControls controls(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto free = host.find<Checkbox>(CameraControls::Names::free);
    ASSERT_NE(free, nullptr);
    ASSERT_FALSE(free->state());
    auto orbit = host.find<Checkbox>(CameraControls::Names::orbit);
    ASSERT_NE(orbit, nullptr);
    ASSERT_FALSE(orbit->state());
    auto axis = host.find<Checkbox>(CameraControls::Names::axis);
    ASSERT_NE(axis, nullptr);
    ASSERT_FALSE(axis->state());

    controls.set_mode(CameraMode::Orbit);

    ASSERT_FALSE(free->state());
    ASSERT_TRUE(orbit->state());
    ASSERT_FALSE(axis->state());
}

TEST(CameraControls, SetModeAxisUpdatesCheckboxes)
{
    ui::Window host(Size(), Colour::Transparent);
    CameraControls controls(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto free = host.find<Checkbox>(CameraControls::Names::free);
    ASSERT_NE(free, nullptr);
    ASSERT_FALSE(free->state());
    auto orbit = host.find<Checkbox>(CameraControls::Names::orbit);
    ASSERT_NE(orbit, nullptr);
    ASSERT_FALSE(orbit->state());
    auto axis = host.find<Checkbox>(CameraControls::Names::axis);
    ASSERT_NE(axis, nullptr);
    ASSERT_FALSE(axis->state());

    controls.set_mode(CameraMode::Axis);

    ASSERT_FALSE(free->state());
    ASSERT_FALSE(orbit->state());
    ASSERT_TRUE(axis->state());
}

TEST(CameraControls, SetProjectionsModeOrthoUpdatesCheckboxes)
{
    ui::Window host(Size(), Colour::Transparent);
    CameraControls controls(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto ortho = host.find<Checkbox>(CameraControls::Names::ortho);
    ASSERT_NE(ortho, nullptr);
    ASSERT_FALSE(ortho->state());

    controls.set_projection_mode(ProjectionMode::Orthographic);

    ASSERT_TRUE(ortho->state());
}

TEST(CameraControls, SetProjectionsModePerspectiveUpdatesCheckboxes)
{
    ui::Window host(Size(), Colour::Transparent);
    CameraControls controls(host, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto ortho = host.find<Checkbox>(CameraControls::Names::ortho);
    ASSERT_NE(ortho, nullptr);
    ASSERT_FALSE(ortho->state());
    ortho->set_state(true);

    controls.set_projection_mode(ProjectionMode::Perspective);

    ASSERT_FALSE(ortho->state());
}
