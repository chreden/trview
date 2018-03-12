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

        auto red = texture_storage.coloured(0xff0000ff);
        auto green = texture_storage.coloured(0xff00ff00);

        auto camera_window = std::make_unique<GroupBox>(Point(), Size(140, 115), Colour(1.0f, 0.5f, 0.5f, 0.5f), Colour(1.0f, 0.0f, 0.0f, 0.0f), L"Camera");
        
        auto reset_camera = std::make_unique<Button>(Point(12, 20), Size(16, 16), red, red);
        reset_camera->on_click += [&](auto) { on_reset(); };

        auto reset_camera_label = std::make_unique<Label>(Point(32, 20), Size(40, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"Reset", 10.0f, TextAlignment::Left, ParagraphAlignment::Centre);

        auto orbit_camera = std::make_unique<Button>(Point(76, 20), Size(16, 16), red, green);
        orbit_camera->on_click += [&](auto) { change_mode(CameraMode::Orbit); };

        auto orbit_camera_label = std::make_unique<Label>(Point(96, 20), Size(40, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"Orbit", 10.0f, TextAlignment::Left, ParagraphAlignment::Centre);

        auto free_camera = std::make_unique<Button>(Point(12, 42), Size(16, 16), red, green);
        free_camera->on_click += [&](auto) { change_mode(CameraMode::Free); };

        auto free_camera_label = std::make_unique<Label>(Point(32, 42), Size(40, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"Free", 10.0f, TextAlignment::Left, ParagraphAlignment::Centre);

        // Camera section for the menu bar.
        auto camera_sensitivity_box = std::make_unique<GroupBox>(Point(12, 64), Size(120, 40), Colour(1.0f, 0.5f, 0.5f, 0.5f), Colour(1.0f, 0.0f, 0.0f, 0.0f), L"Sensitivity");
        auto camera_sensitivity = std::make_unique<ui::Slider>(Point(6, 12), Size(108, 16));
        camera_sensitivity->on_value_changed += on_sensitivity_changed;

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

    void CameraControls::change_mode(CameraMode mode)
    {
        _orbit->set_state(mode == CameraMode::Orbit);
        _free->set_state(mode == CameraMode::Free);
    }

    void CameraControls::set_sensitivity(float value)
    {
        _sensitivity->set_value(value);
    }
}
