#include "stdafx.h"
#include "SettingsWindow.h"

#include <trview.ui/Control.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Label.h>
#include <trview.ui/Button.h>

using namespace trview::ui;

namespace trview
{
    SettingsWindow::SettingsWindow(Control& parent)
    {
        const auto background_colour = Colour(1.0f, 0.5f, 0.5f, 0.5f);
        const auto title_colour = Colour(1.0f, 0.3f, 0.3f, 0.3f);

        auto window = std::make_unique<StackPanel>(Point(400, 200), Size(400, 300), background_colour, Size());
        window->set_visible(false);

        // Create the title bar.
        auto title_bar = std::make_unique<StackPanel>(Point(), Size(400, 20), title_colour, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
        auto title = std::make_unique<Label>(Point(), Size(400, 20), title_colour, L"Settings", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre);
        title->set_horizontal_alignment(Align::Centre);
        title_bar->add_child(std::move(title));
        window->add_child(std::move(title_bar));

        // Create the rest of the window contents.
        auto panel = std::make_unique<StackPanel>(Point(), Size(400, 250), background_colour, Size(5,5));
        panel->set_auto_size_dimension(SizeDimension::Height);
        panel->set_margin(Size(5, 5));

        auto vsync = std::make_unique<Checkbox>(Point(), Size(16, 16), L"Vsync");
        vsync->on_state_changed += on_vsync;
        _vsync = panel->add_child(std::move(vsync));

        auto go_to_lara = std::make_unique<Checkbox>(Point(), Size(16, 16), L"Go to Lara");
        go_to_lara->on_state_changed += on_go_to_lara;
        _go_to_lara = panel->add_child(std::move(go_to_lara));

        auto invert_map_controls = std::make_unique<Checkbox>(Point(), Size(16, 16), L"Invert map controls");
        invert_map_controls->on_state_changed += on_invert_map_controls;
        _invert_map_controls = panel->add_child(std::move(invert_map_controls));

        auto items_startup = std::make_unique<Checkbox>(Point(), Size(16, 16), L"Open Items Window at startup");
        items_startup->on_state_changed += on_items_startup;
        _items_startup = panel->add_child(std::move(items_startup));

        auto triggers_startup = std::make_unique<Checkbox>(Point(), Size(16, 16), L"Open Triggers Window at startup");
        triggers_startup->on_state_changed += on_triggers_startup;
        _triggers_startup = panel->add_child(std::move(triggers_startup));

        auto auto_orbit = std::make_unique<Checkbox>(Point(), Size(16, 16), L"Switch to orbit on selection");
        auto_orbit->on_state_changed += on_auto_orbit;
        _auto_orbit = panel->add_child(std::move(auto_orbit));

        auto ok = std::make_unique<Button>(Point(), Size(60, 20), L"Close");
        ok->set_horizontal_alignment(Align::Centre);
        _token_store.add(ok->on_click += [&]() { _window->set_visible(!_window->visible()); });
        panel->add_child(std::move(ok));

        window->add_child(std::move(panel));
        _window = parent.add_child(std::move(window));

        // Register for control resizes on the parent so that the window will always
        // be in the middle of the screen.
        auto centre_window = [&](const Size& parent_size)
        {
            const auto half_size = _window->size() / 2.0f;
            _window->set_position(
                Point(parent_size.width / 2.0f - half_size.width,
                      parent_size.height / 2.0f - half_size.height));
        };
        parent.on_size_changed += centre_window;
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

    void SettingsWindow::set_auto_orbit(bool value)
    {
        _auto_orbit->set_state(value);
    }

    void SettingsWindow::toggle_visibility()
    {
        _window->set_visible(!_window->visible());
    }
}
