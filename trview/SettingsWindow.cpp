#include "stdafx.h"
#include "SettingsWindow.h"

#include <trview.ui/Control.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Label.h>
#include <trview.ui/Button.h>

#include "ITextureStorage.h"

using namespace trview::ui;

namespace trview
{
    SettingsWindow::SettingsWindow(Control& parent, const ITextureStorage& texture_storage)
    {
        const auto background_colour = Colour(1.0f, 0.5f, 0.5f, 0.5f);
        const auto title_colour = Colour(1.0f, 0.3f, 0.3f, 0.3f);

        auto window = std::make_unique<StackPanel>(Point(400, 200), Size(400, 300), background_colour, Size());
        window->set_visible(false);
        _window = window.get();

        // Create the title bar.
        auto title_bar = std::make_unique<StackPanel>(Point(), Size(400, 20), title_colour, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
        auto title = std::make_unique<Label>(Point(), Size(400, 20), title_colour, L"Settings", 8.0f, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre);
        title->set_horizontal_alignment(Align::Centre);
        title_bar->add_child(std::move(title));
        window->add_child(std::move(title_bar));

        // Create the rest of the window contents.
        auto panel = std::make_unique<StackPanel>(Point(), Size(400, 250), background_colour, Size(5,5));
        panel->set_auto_size_dimension(SizeDimension::Height);

        const auto check_off = texture_storage.lookup("check_off");
        const auto check_on = texture_storage.lookup("check_on");

        auto vsync = std::make_unique<Checkbox>(Point(), Size(16, 16), check_off, check_on, L"Vsync");
        vsync->on_state_changed += on_vsync;
        _vsync = vsync.get();
        panel->add_child(std::move(vsync));

        auto go_to_lara = std::make_unique<Checkbox>(Point(), Size(16, 16), check_off, check_on, L"Go to Lara");
        go_to_lara->on_state_changed += on_go_to_lara;
        _go_to_lara = go_to_lara.get();
        panel->add_child(std::move(go_to_lara));

        auto invert_map_controls = std::make_unique<Checkbox>(Point(), Size(16, 16), check_off, check_on, L"Invert map controls");
        invert_map_controls->on_state_changed += on_invert_map_controls;
        _invert_map_controls = invert_map_controls.get();
        panel->add_child(std::move(invert_map_controls));

        auto ok = std::make_unique<Button>(Point(), Size(60, 20), L"Close");
        ok->set_horizontal_alignment(Align::Centre);
        ok->on_click += [&]() { _window->set_visible(!_window->visible()); };
        panel->add_child(std::move(ok));

        window->add_child(std::move(panel));
        parent.add_child(std::move(window));

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

    void SettingsWindow::toggle_visibility()
    {
        _window->set_visible(!_window->visible());
    }
}
