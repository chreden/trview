#include "ViewOptions.h"

#include <trview.ui/Button.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/NumericUpDown.h>
#include "../Resources/resource.h"

namespace trview
{
    using namespace ui;

    const std::string ViewOptions::Names::depth{ "depth" };
    const std::string ViewOptions::Names::group{ "group" };
    const std::string ViewOptions::Names::tr_1_3_panel{ "tr1-3-panel" };
    const std::string ViewOptions::Names::tr_4_5_panel{ "tr4-5-panel" };
    const std::string ViewOptions::Names::alternate_groups{ "alternate_groups" };

    const Colour ViewOptions::Colours::FlipOff{ 0.2f, 0.2f, 0.2f };
    const Colour ViewOptions::Colours::FlipOn{ 0.6f, 0.6f, 0.6f };

    ViewOptions::ViewOptions(ui::Control& parent, const std::shared_ptr<ui::ILoader>& source)
    {
        using namespace ui;

        auto options = parent.add_child(source->load_from_resource(IDR_UI_VIEW_OPTIONS));

        _depth = options->find<NumericUpDown>(Names::depth);
        _depth->on_value_changed += on_depth_changed;

        _tr1_3_panel = options->find<ui::Window>(Names::tr_1_3_panel);
        _tr4_5_panel = options->find<ui::Window>(Names::tr_4_5_panel);
        _alternate_groups = options->find<ui::Window>(Names::alternate_groups);

        find_toggles(*options);
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
            button->set_name(Names::group + std::to_string(group));
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

    void ViewOptions::set_use_alternate_groups(bool value)
    {
        _tr1_3_panel->set_visible(!value);
        _tr4_5_panel->set_visible(value);
    }

    void ViewOptions::find_toggles(Control& options)
    {
        auto toggles = options.find_all<Checkbox>();
        for (auto toggle : toggles)
        {
            _toggles[toggle->name()] = toggle;
            _token_store += toggle->on_state_changed += [this, toggle](bool value)
            {
                on_toggle_changed(toggle->name(), value);
            };
        }
    }

    void ViewOptions::set_toggle(const std::string& name, bool value) 
    {
        auto toggle = _toggles.find(name);
        if (toggle == _toggles.end())
        {
            return;
        }
        toggle->second->set_state(value);
    }

    bool ViewOptions::toggle(const std::string& name) const 
    {
        auto toggle = _toggles.find(name);
        if (toggle == _toggles.end())
        {
            return false;
        }
        return toggle->second->state();
    }

    void ViewOptions::set_flip_enabled(bool enabled)
    {
        auto flip = _toggles.find("flip");
        if (flip != _toggles.end())
        {
            flip->second->set_enabled(enabled);
        }
    }
}
