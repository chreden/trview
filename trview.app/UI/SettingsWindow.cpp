#include "SettingsWindow.h"

#include <trview.ui/Control.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Label.h>
#include <trview.ui/Button.h>
#include <trview.ui/Slider.h>
#include <trview.ui/NumericUpDown.h>
#include "../Resources/resource.h"

using namespace trview::ui;

namespace trview
{
    const std::string SettingsWindow::Names::vsync = "VSync";
    const std::string SettingsWindow::Names::go_to_lara = "GoToLara";
    const std::string SettingsWindow::Names::invert_map_controls = "InvertMapControls";
    const std::string SettingsWindow::Names::items_startup = "ItemsStartup";
    const std::string SettingsWindow::Names::triggers_startup = "TriggersStartup";
    const std::string SettingsWindow::Names::rooms_startup = "RoomsStartup";
    const std::string SettingsWindow::Names::auto_orbit = "AutoOrbit";
    const std::string SettingsWindow::Names::invert_vertical_pan = "InvertVerticalPan";
    const std::string SettingsWindow::Names::camera_display_degrees = "CameraDisplayDegrees";
    const std::string SettingsWindow::Names::randomizer_tools = "RandomizerTools";
    const std::string SettingsWindow::Names::max_recent_files = "MaxRecentFiles";
    const std::string SettingsWindow::Names::sensitivity = "Sensitivity";
    const std::string SettingsWindow::Names::movement_speed = "MovementSpeed";
    const std::string SettingsWindow::Names::acceleration = "Acceleration";
    const std::string SettingsWindow::Names::acceleration_rate = "AccelerationRate";
    const std::string SettingsWindow::Names::close = "Close";

    SettingsWindow::SettingsWindow(Control& parent, const std::shared_ptr<ui::ILoader>& source)
    {
        _window = parent.add_child(source->load_from_resource(IDR_UI_SETTINGS_WINDOW));

        _vsync = _window->find<Checkbox>(Names::vsync);
        _vsync->on_state_changed += on_vsync;

        _go_to_lara = _window->find<Checkbox>(Names::go_to_lara);
        _go_to_lara->on_state_changed += on_go_to_lara;

        _invert_map_controls = _window->find<Checkbox>(Names::invert_map_controls);
        _invert_map_controls->on_state_changed += on_invert_map_controls;

        _items_startup = _window->find<Checkbox>(Names::items_startup);
        _items_startup->on_state_changed += on_items_startup;

        _triggers_startup = _window->find<Checkbox>(Names::triggers_startup);
        _triggers_startup->on_state_changed += on_triggers_startup;

        _rooms_startup = _window->find<Checkbox>(Names::rooms_startup);
        _rooms_startup->on_state_changed += on_rooms_startup;

        _auto_orbit = _window->find<Checkbox>(Names::auto_orbit);
        _auto_orbit->on_state_changed += on_auto_orbit;

        _invert_vertical_pan = _window->find<Checkbox>(Names::invert_vertical_pan);
        _invert_vertical_pan->on_state_changed += on_invert_vertical_pan;

        _camera_display_degrees = _window->find<Checkbox>(Names::camera_display_degrees);
        _camera_display_degrees->on_state_changed += on_camera_display_degrees;

        _randomizer_tools = _window->find<Checkbox>(Names::randomizer_tools);
        _randomizer_tools->on_state_changed += on_randomizer_tools;

        _max_recent_files = _window->find<NumericUpDown>(Names::max_recent_files);
        _token_store += _max_recent_files->on_value_changed += [this](int32_t value) { on_max_recent_files(value); };

        _sensitivity = _window->find<Slider>(Names::sensitivity);
        _sensitivity->on_value_changed += on_sensitivity_changed;

        _movement_speed = _window->find<Slider>(Names::movement_speed);
        _movement_speed->on_value_changed += on_movement_speed_changed;

        _acceleration = _window->find<Checkbox>(Names::acceleration);
        _acceleration->on_state_changed += on_camera_acceleration;

        _acceleration_rate = _window->find<Slider>(Names::acceleration_rate);
        _acceleration_rate->on_value_changed += on_camera_acceleration_rate;

        auto close = _window->find<Button>(Names::close);
        _token_store += close->on_click += [&]() { toggle_visibility(); };

        // Register for control resizes on the parent so that the window will always
        // be in the middle of the screen.
        auto centre_window = [&](const Size& parent_size)
        {
            const auto half_size = _window->size() / 2.0f;
            _window->set_position(
                Point(parent_size.width / 2.0f - half_size.width,
                      parent_size.height / 2.0f - half_size.height));
        };
        _token_store += parent.on_size_changed += centre_window;
        centre_window(parent.size());
    }

    void SettingsWindow::set_vsync(bool value)
    {
        _vsync->set_state(value);
    }

    void SettingsWindow::set_go_to_lara(bool value)
    {
        _go_to_lara->set_state(value);
    }

    void SettingsWindow::set_invert_map_controls(bool value)
    {
        _invert_map_controls->set_state(value);
    }

    void SettingsWindow::set_items_startup(bool value)
    {
        _items_startup->set_state(value);
    }

    void SettingsWindow::set_triggers_startup(bool value)
    {
        _triggers_startup->set_state(value);
    }

    void SettingsWindow::set_rooms_startup(bool value)
    {
        _rooms_startup->set_state(value);
    }

    void SettingsWindow::set_auto_orbit(bool value)
    {
        _auto_orbit->set_state(value);
    }

    void SettingsWindow::set_camera_acceleration(bool value)
    {
        _acceleration->set_state(value);
    }

    void SettingsWindow::set_camera_acceleration_rate(float value)
    {
        _acceleration_rate->set_value(value);
    }

    void SettingsWindow::toggle_visibility()
    {
        _window->set_visible(!_window->visible());
    }

    void SettingsWindow::set_sensitivity(float value)
    {
        _sensitivity->set_value(value);
    }

    void SettingsWindow::set_movement_speed(float value)
    {
        _movement_speed->set_value(value);
    }

    void SettingsWindow::set_invert_vertical_pan(bool value)
    {
        _invert_vertical_pan->set_state(value);
    }

    void SettingsWindow::set_camera_display_degrees(bool value)
    {
        _camera_display_degrees->set_state(value);
    }

    void SettingsWindow::set_randomizer_tools(bool value)
    {
        _randomizer_tools->set_state(value);
    }

    void SettingsWindow::set_max_recent_files(uint32_t value)
    {
        _max_recent_files->set_value(value);
    }
}
