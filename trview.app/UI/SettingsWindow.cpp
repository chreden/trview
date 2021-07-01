#include "SettingsWindow.h"

#include <trview.ui/Control.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Label.h>
#include <trview.ui/Button.h>
#include <trview.ui/Slider.h>
#include <trview.ui/Grid.h>

using namespace trview::ui;

namespace trview
{
    SettingsWindow::SettingsWindow(Control& parent)
    {
        const auto background_colour = Colour(0.5f, 0.0f, 0.0f, 0.0f);
        const auto title_colour = Colour::Black;

        _window = parent.add_child(std::make_unique<StackPanel>(Point(400, 200), Size(400, 300), background_colour, Size()));
        _window->set_visible(false);
        _window->set_name("SettingsWindow");

        // Create the title bar.
        auto title_bar = _window->add_child(std::make_unique<StackPanel>(Size(400, 20), title_colour, Size(), StackPanel::Direction::Vertical, SizeMode::Manual));
        auto title = title_bar->add_child(std::make_unique<Label>(Size(400, 20), title_colour, L"Settings", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre));
        title->set_horizontal_alignment(Align::Centre);

        // Create the rest of the window contents.
        auto panel = _window->add_child(std::make_unique<StackPanel>(Size(400, 250), Colour::Transparent , Size(5, 5)));
        panel->set_auto_size_dimension(SizeDimension::Height);
        panel->set_margin(Size(5, 5));

        _vsync = panel->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Vsync"));
        _vsync->set_name("VSync");
        _vsync->on_state_changed += on_vsync;

        _go_to_lara = panel->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Go to Lara"));
        _go_to_lara->set_name("GoToLara");
        _go_to_lara->on_state_changed += on_go_to_lara;

        _invert_map_controls = panel->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Invert map controls"));
        _invert_map_controls->set_name("InvertMapControls");
        _invert_map_controls->on_state_changed += on_invert_map_controls;

        _items_startup = panel->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Open Items Window at startup"));
        _items_startup->set_name("ItemsStartup");
        _items_startup->on_state_changed += on_items_startup;

        _triggers_startup = panel->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Open Triggers Window at startup"));
        _triggers_startup->set_name("TriggersStartup");
        _triggers_startup->on_state_changed += on_triggers_startup;

        _rooms_startup = panel->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Open Rooms Window at startup"));
        _rooms_startup->set_name("RoomsStartup");
        _rooms_startup->on_state_changed += on_rooms_startup;

        _auto_orbit = panel->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Switch to orbit on selection"));
        _auto_orbit->set_name("AutoOrbit");
        _auto_orbit->on_state_changed += on_auto_orbit;

        _invert_vertical_pan = panel->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Invert vertical panning"));
        _invert_vertical_pan->set_name("InvertVerticalPan");
        _invert_vertical_pan->on_state_changed += on_invert_vertical_pan;

        _camera_display_degrees = panel->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Use degrees for camera angle display"));
        _camera_display_degrees->set_name("CameraDisplayDegrees");
        _camera_display_degrees->on_state_changed += on_camera_display_degrees;

        auto camera_group = panel->add_child(std::make_unique<GroupBox>(Size(380, 80), Colour::Transparent, Colour::LightGrey, L"Camera Movement"));
        auto camera_panel = camera_group->add_child(std::make_unique<Grid>(Size(360, 50), Colour::Transparent, 2, 2));

        auto add_labelled_slider = [&](const std::wstring& text)
        {
            auto slider_panel = camera_panel->add_child(std::make_unique<StackPanel>(Size(160, 50), Colour::Transparent, Size(), StackPanel::Direction::Horizontal));
            slider_panel->add_child(std::make_unique<Label>(Size(), Colour::Transparent, text, 8, graphics::TextAlignment::Left, graphics::ParagraphAlignment::Near, SizeMode::Auto));
            return slider_panel->add_child(std::make_unique<Slider>(Size(100, 16)));
        };

        _sensitivity = add_labelled_slider(L"Sensitivity");
        _sensitivity->set_name("Sensitivity");
        _sensitivity->on_value_changed += on_sensitivity_changed;

        _movement_speed = add_labelled_slider(L"Movement Speed ");
        _movement_speed->set_name("MovementSpeed");
        _movement_speed->on_value_changed += on_movement_speed_changed;

        _acceleration = camera_panel->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Acceleration"));
        _acceleration->set_name("Acceleration");
        _acceleration->on_state_changed += on_camera_acceleration;

        _acceleration_rate = add_labelled_slider(L"Acceleration Rate");
        _acceleration_rate->set_name("AccelerationRate");
        _acceleration_rate->on_value_changed += on_camera_acceleration_rate;

        auto close = panel->add_child(std::make_unique<Button>(Point(), Size(60, 20), L"Close"));
        close->set_horizontal_alignment(Align::Centre);
        close->set_name("Close");
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
}
