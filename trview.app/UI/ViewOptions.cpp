#include "ViewOptions.h"

#include <trview.ui/Button.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/NumericUpDown.h>
#include <trview.ui/Grid.h>
#include <trview.app/Graphics/ITextureStorage.h>

namespace trview
{
    namespace Colours
    {
        const Colour FlipOff{ 0.2f, 0.2f, 0.2f };
        const Colour FlipOn{ 0.6f, 0.6f, 0.6f };
    }

    ViewOptions::ViewOptions(ui::Control& parent, const ITextureStorage& texture_storage)
    {
        using namespace ui;

        auto rooms_group = parent.add_child(std::make_unique<GroupBox>(Size(150, 155), Colour::Transparent, Colour::Grey, L"View Options"));
        auto rooms_area = rooms_group->add_child(std::make_unique<StackPanel>(Size(150, 155), Colour::Transparent));
        auto rooms_grid = rooms_area->add_child(std::make_unique<Grid>(Size(150, 95), Colour::Transparent, 2, 4));

        _highlight = rooms_grid->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Highlight"));
        _highlight->on_state_changed += on_highlight;

        _triggers = rooms_grid->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Triggers"));
        _triggers->set_state(true);
        _triggers->on_state_changed += on_show_triggers;

        _hidden_geometry = rooms_grid->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Geometry"));
        _hidden_geometry->on_state_changed += on_show_hidden_geometry;

        _water = rooms_grid->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Water"));
        _water->set_state(true);
        _water->on_state_changed += on_show_water;

        _enabled = rooms_grid->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Depth"));
        _enabled->on_state_changed += on_depth_enabled;

        _depth = rooms_grid->add_child(std::make_unique<NumericUpDown>(Size(50, 20), Colour::Transparent, texture_storage.lookup("numeric_up"), texture_storage.lookup("numeric_down"), 0, 20));
        _depth->set_value(1);
        _depth->on_value_changed += on_depth_changed;

        _wireframe = rooms_grid->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Wireframe"));
        _wireframe->on_state_changed += on_show_wireframe;
        
        const auto panel_size = Size(140, 20);
        auto flip_panel = rooms_area->add_child(std::make_unique<ui::Window>(panel_size, Colour::Transparent));
        _tr1_3_panel = flip_panel->add_child(std::make_unique<ui::Window>(panel_size, Colour::Transparent));
        _flip = _tr1_3_panel->add_child(std::make_unique<Checkbox>(Colour::Transparent, L"Flip"));
        _flip->on_state_changed += on_flip;

        _tr4_5_panel = flip_panel->add_child(std::make_unique<ui::Window>(panel_size, Colour::Transparent));
        _alternate_groups = _tr4_5_panel->add_child(std::make_unique<StackPanel>(Size(140, 16), Colour::Transparent, Size(), StackPanel::Direction::Horizontal, SizeMode::Manual));
        _tr4_5_panel->set_visible(false);
    }

    void ViewOptions::set_alternate_group(uint32_t value, bool enabled)
    {
        _alternate_group_values[value] = enabled;

        // Update the button.
        auto button = _alternate_group_buttons.find(value);
        if (button != _alternate_group_buttons.end())
        {
            button->second->set_text_background_colour(enabled ? Colours::FlipOn : Colours::FlipOff);
        }
    }

    void ViewOptions::set_alternate_groups(const std::set<uint32_t>& groups)
    {
        _alternate_groups->clear_child_elements();
        _alternate_group_values.clear();
        _alternate_group_buttons.clear();

        for (auto& group : groups)
        {
            _alternate_group_values.insert({ group, false });

            auto button = std::make_unique<ui::Button>(Size(16, 16), std::to_wstring(group));
            button->set_text_background_colour(Colours::FlipOff);
            auto group_button = _alternate_groups->add_child(std::move(button));

            _token_store += group_button->on_click += [group_button, this, group]()
            {
                auto group_value = !_alternate_group_values[group];
                group_button->set_text_background_colour(group_value ? Colours::FlipOn : Colours::FlipOff);
                on_alternate_group(group, group_value);
                _alternate_group_values[group] = group_value;
            };

            _alternate_group_buttons[group] = group_button;
        }
    }

    void ViewOptions::set_depth(int32_t value)
    {
        _depth->set_value(value);
    }
    
    void ViewOptions::set_depth_enabled(bool value)
    {
        _enabled->set_state(value);
    }

    void ViewOptions::set_flip(bool flip)
    {
        _flip->set_state(flip);
    }

    void ViewOptions::set_flip_enabled(bool enabled)
    {
        _flip->set_enabled(enabled);
    }

    void ViewOptions::set_highlight(bool highlight)
    {
        _highlight->set_state(highlight);
    }

    void ViewOptions::set_show_hidden_geometry(bool show)
    {
        _hidden_geometry->set_state(show);
    }

    void ViewOptions::set_show_triggers(bool show)
    {
        _triggers->set_state(show);
    }

    void ViewOptions::set_show_water(bool show)
    {
        _water->set_state(show);
    }

    void ViewOptions::set_show_wireframe(bool show)
    {
        _wireframe->set_state(show);
    }

    void ViewOptions::set_use_alternate_groups(bool value)
    {
        _tr1_3_panel->set_visible(!value);
        _tr4_5_panel->set_visible(value);
    }

    bool ViewOptions::show_hidden_geometry() const
    {
        return _hidden_geometry->state();
    }

    bool ViewOptions::show_triggers() const
    {
        return _triggers->state();
    }

    bool ViewOptions::show_water() const
    {
        return _water->state();
    }    

    bool ViewOptions::show_wireframe() const
    {
        return _wireframe->state();
    }
}
