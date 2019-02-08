#include "stdafx.h"
#include "Flipmaps.h"
#include <trview.ui/GroupBox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Button.h>
#include <string>

namespace trview
{
    namespace Colours
    {
        const Colour FlipOff { 0.2f, 0.2f, 0.2f };
        const Colour FlipOn { 0.6f, 0.6f, 0.6f };
    }

    Flipmaps::Flipmaps(ui::Control& parent)
    {
        using namespace ui;
        auto flips_group = std::make_unique<GroupBox>(Point(), Size(140, 45), Colour(0.5f, 0.5f, 0.5f), Colour(0.0f, 0.0f, 0.0f), L"Flipmaps");

        // Shared panel size.
        const auto panel_size = Size(130, 20);

        // Add two methods of controlling flipmaps:
        // The TR1-3 method:
        auto tr1_3_panel = std::make_unique<ui::Window>(Point(5, 16), panel_size, Colour(0.5f, 0.5f, 0.5f));
        auto flip = std::make_unique<Checkbox>(Point(7,0), Size(16, 16), L"Flip");
        flip->on_state_changed += on_flip;
        _flip = tr1_3_panel->add_child(std::move(flip));
        _tr1_3_panel = flips_group->add_child(std::move(tr1_3_panel));

        // The TR4-5 method:
        auto tr4_5_panel = std::make_unique<ui::Window>(Point(5, 16), panel_size, Colour(0.5f, 0.5f, 0.5f));
        auto alternate_groups = std::make_unique<StackPanel>(Point(), Size(130, 16), Colour(0.5f, 0.5f, 0.5f), Size(), StackPanel::Direction::Horizontal, SizeMode::Manual);
        _alternate_groups = tr4_5_panel->add_child(std::move(alternate_groups));
        tr4_5_panel->set_visible(false);
        _tr4_5_panel = flips_group->add_child(std::move(tr4_5_panel));

        parent.add_child(std::move(flips_group));
    }

    void Flipmaps::set_use_alternate_groups(bool value)
    {
        _tr1_3_panel->set_visible(!value);
        _tr4_5_panel->set_visible(value);
    }

    void Flipmaps::set_alternate_group(uint16_t value, bool enabled)
    {
        _alternate_group_values[value] = enabled;

        // Update the button.
        auto button = _alternate_group_buttons.find(value);
        if (button != _alternate_group_buttons.end())
        {
            button->second->set_text_background_colour(enabled ? Colours::FlipOn : Colours::FlipOff);
        }
    }

    void Flipmaps::set_alternate_groups(const std::set<uint16_t>& groups)
    {
        _alternate_groups->clear_child_elements();
        _alternate_group_values.clear();
        _alternate_group_buttons.clear();

        for (auto& group : groups)
        {
            _alternate_group_values.insert({ group, false });

            auto button = std::make_unique<ui::Button>(Point(), Size(16, 16), std::to_wstring(group));
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

    void Flipmaps::set_flip(bool flip)
    {
        _flip->set_state(flip);
    }

    void Flipmaps::set_flip_enabled(bool enabled)
    {
        _flip->set_enabled(enabled);
    }
}
