#include "ViewOptions.h"

#include <trview.ui/Button.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/NumericUpDown.h>
#include "../Resources/resource.h"

namespace trview
{
    const std::string ViewOptions::Names::hidden_geometry{ "hidden_geometry" };
    const std::string ViewOptions::Names::highlight{ "highlight" };
    const std::string ViewOptions::Names::depth_enabled{ "depth_enabled" };
    const std::string ViewOptions::Names::depth{ "depth" };
    const std::string ViewOptions::Names::triggers{ "triggers" };
    const std::string ViewOptions::Names::show_bounding_boxes { "show_bounding_boxes" };
    const std::string ViewOptions::Names::water{ "water" };
    const std::string ViewOptions::Names::wireframe{ "wireframe" };
    const std::string ViewOptions::Names::flip{ "flip" };
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

        _highlight = options->find<Checkbox>(Names::highlight);
        _highlight->on_state_changed += on_highlight;

        _triggers = options->find<Checkbox>(Names::triggers);
        _triggers->on_state_changed += on_show_triggers;

        _hidden_geometry = options->find<Checkbox>(Names::hidden_geometry);
        _hidden_geometry->on_state_changed += on_show_hidden_geometry;

        _water = options->find<Checkbox>(Names::water);
        _water->on_state_changed += on_show_water;

        _depth_enabled = options->find<Checkbox>(Names::depth_enabled);
        _depth_enabled->on_state_changed += on_depth_enabled;

        _depth = options->find<NumericUpDown>(Names::depth);
        _depth->on_value_changed += on_depth_changed;

        _wireframe = options->find<Checkbox>(Names::wireframe);
        _wireframe->on_state_changed += on_show_wireframe;

        _bounding_boxes = options->find<Checkbox>(Names::show_bounding_boxes);
        _bounding_boxes->on_state_changed += on_show_bounding_boxes;

        _tr1_3_panel = options->find<ui::Window>(Names::tr_1_3_panel);
        _flip = options->find<Checkbox>(Names::flip);
        _flip->on_state_changed += on_flip;
        _tr4_5_panel = options->find<ui::Window>(Names::tr_4_5_panel);
        _alternate_groups = options->find<ui::Window>(Names::alternate_groups);
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
    
    void ViewOptions::set_depth_enabled(bool value)
    {
        _depth_enabled->set_state(value);
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

    void ViewOptions::set_show_bounding_boxes(bool value)
    {
        _bounding_boxes->set_state(value);
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

    bool ViewOptions::show_bounding_boxes() const
    {
        return _bounding_boxes->state();
    }
}
