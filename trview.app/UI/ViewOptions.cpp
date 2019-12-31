#include "ViewOptions.h"

#include <trview.ui/Button.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/NumericUpDown.h>
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

        auto rooms_groups = std::make_unique<GroupBox>(Size(150, 130), Colour::Transparent, Colour::Grey, L"View Options");
        auto highlight = std::make_unique<Checkbox>(Point(12, 20), Colour::Transparent, L"Highlight");
        auto triggers = std::make_unique<Checkbox>(Point(86, 20), Colour::Transparent, L"Triggers");
        triggers->set_state(true);
        auto hidden_geometry = std::make_unique<Checkbox>(Point(12, 45), Colour::Transparent, L"Geometry");
        auto water = std::make_unique<Checkbox>(Point(86, 45), Colour::Transparent, L"Water");
        water->set_state(true);

        highlight->on_state_changed += on_highlight;
        triggers->on_state_changed += on_show_triggers;
        hidden_geometry->on_state_changed += on_show_hidden_geometry;
        water->on_state_changed += on_show_water;

        auto enabled = std::make_unique<Checkbox>(Point(12, 70), Colour::Transparent, L"Depth");
        enabled->on_state_changed += on_depth_enabled;

        auto depth = std::make_unique<NumericUpDown>(Point(86, 70), Size(50, 20), Colour::Transparent, texture_storage.lookup("numeric_up"), texture_storage.lookup("numeric_down"), 0, 20);
        depth->set_value(1);
        depth->on_value_changed += on_depth_changed;

        _enabled = rooms_groups->add_child(std::move(enabled));
        _depth = rooms_groups->add_child(std::move(depth));

        _highlight = rooms_groups->add_child(std::move(highlight));
        _triggers = rooms_groups->add_child(std::move(triggers));
        _hidden_geometry = rooms_groups->add_child(std::move(hidden_geometry));
        _water = rooms_groups->add_child(std::move(water));

        // Shared panel size.
        const auto panel_size = Size(140, 20);

        // Add two methods of controlling flipmaps:
        // The TR1-3 method:
        auto tr1_3_panel = std::make_unique<ui::Window>(Point(12, 96), panel_size, Colour::Transparent);
        auto flip = std::make_unique<Checkbox>(Colour::Transparent, L"Flip");
        flip->on_state_changed += on_flip;
        _flip = tr1_3_panel->add_child(std::move(flip));
        _tr1_3_panel = rooms_groups->add_child(std::move(tr1_3_panel));

        // The TR4-5 method:
        auto tr4_5_panel = std::make_unique<ui::Window>(Point(12, 96), panel_size, Colour::Transparent);
        auto alternate_groups = std::make_unique<StackPanel>(Size(140, 16), Colour::Transparent, Size(), StackPanel::Direction::Horizontal, SizeMode::Manual);
        _alternate_groups = tr4_5_panel->add_child(std::move(alternate_groups));
        tr4_5_panel->set_visible(false);
        _tr4_5_panel = rooms_groups->add_child(std::move(tr4_5_panel));

        parent.add_child(std::move(rooms_groups));
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
}
