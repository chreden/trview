#include "stdafx.h"
#include "CameraControls.h"

#include "ITextureStorage.h"

#include <trview.ui/GroupBox.h>
#include <trview.ui/Button.h>
#include <trview.ui/Slider.h>
#include <trview.ui/Label.h>

namespace trview
{
    CameraControls::CameraControls(ui::Control& parent, const ITextureStorage& texture_storage)
    {
        using namespace ui;

        auto up = texture_storage.lookup("check_off");
        auto down = texture_storage.lookup("check_on");

        auto camera_window = std::make_unique<GroupBox>(Point(), Size(140, 115), Colour(1.0f, 0.5f, 0.5f, 0.5f), Colour(1.0f, 0.0f, 0.0f, 0.0f), L"Camera");

        auto reset_camera = std::make_unique<Button>(Point(12, 20), Size(16, 16), texture_storage.lookup("button_up"), texture_storage.lookup("button_up"));
        reset_camera->on_click += [&](auto) { on_reset(); };

        auto reset_camera_label = std::make_unique<Label>(Point(32, 20), Size(40, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"Reset", 10.0f, TextAlignment::Left, ParagraphAlignment::Centre);

        auto orbit_camera = std::make_unique<Button>(Point(76, 20), Size(16, 16), up, down);
        orbit_camera->on_click += [&](auto) { change_mode(CameraMode::Orbit); };

        auto orbit_camera_label = std::make_unique<Label>(Point(96, 20), Size(40, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"Orbit", 10.0f, TextAlignment::Left, ParagraphAlignment::Centre);

        auto free_camera = std::make_unique<Button>(Point(12, 42), Size(16, 16), up, down);
        free_camera->on_click += [&](auto) { change_mode(CameraMode::Free); };

        auto free_camera_label = std::make_unique<Label>(Point(32, 42), Size(40, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"Free", 10.0f, TextAlignment::Left, ParagraphAlignment::Centre);

        // Camera section for the menu bar.
        auto camera_sensitivity_box = std::make_unique<GroupBox>(Point(12, 64), Size(120, 40), Colour(1.0f, 0.5f, 0.5f, 0.5f), Colour(1.0f, 0.0f, 0.0f, 0.0f), L"Sensitivity");
        auto camera_sensitivity = std::make_unique<ui::Slider>(Point(6, 12), Size(108, 16));
        camera_sensitivity->on_value_changed += [&](float value)
        {
            on_sensitivity_changed(value);
        };

        // Take a copy of buttons that need to be tracked.
        _orbit = orbit_camera.get();
        _free = free_camera.get();
        _sensitivity = camera_sensitivity.get();

        camera_sensitivity_box->add_child(std::move(camera_sensitivity));

        camera_window->add_child(std::move(reset_camera));
        camera_window->add_child(std::move(reset_camera_label));
        camera_window->add_child(std::move(orbit_camera));
        camera_window->add_child(std::move(orbit_camera_label));
        camera_window->add_child(std::move(free_camera));
        camera_window->add_child(std::move(free_camera_label));
        camera_window->add_child(std::move(camera_sensitivity_box));

        parent.add_child(std::move(camera_window));
    }

    // Set the current camera mode and raise the on_mode_selected event.
    // mode: The new camera mode.
    void CameraControls::change_mode(CameraMode mode)
    {
        set_mode(mode);
        on_mode_selected(mode);
    }

    // Set the sensitivity slider to the specified value. This will not raise the on_sensitivity_changed event.
    // value: The sensitivity value.
    void CameraControls::set_sensitivity(float value)
    {
        _sensitivity->set_value(value);
    }

    // Set the current camera mode. This will not raise the on_mode_selected event.
    // mode: The camera mode to change to.
    void CameraControls::set_mode(CameraMode mode)
    {
        _orbit->set_state(mode == CameraMode::Orbit);
        _free->set_state(mode == CameraMode::Free);
    }
}
