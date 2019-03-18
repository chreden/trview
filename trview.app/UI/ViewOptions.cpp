#include "ViewOptions.h"

#include <trview.ui/GroupBox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/NumericUpDown.h>
#include <trview.app/Graphics/ITextureStorage.h>

namespace trview
{
    ViewOptions::ViewOptions(ui::Control& parent, const ITextureStorage& texture_storage)
    {
        using namespace ui;

        auto rooms_groups = std::make_unique<GroupBox>(Point(), Size(150, 110), Colour::Transparent, Colour::Grey, L"View Options");
        auto highlight = std::make_unique<Checkbox>(Point(12, 20), Size(16, 16), Colour::Transparent, L"Highlight");
        auto triggers = std::make_unique<Checkbox>(Point(86, 20), Size(16, 16), Colour::Transparent, L"Triggers");
        triggers->set_state(true);
        auto hidden_geometry = std::make_unique<Checkbox>(Point(12, 45), Size(16, 16), Colour::Transparent, L"Geometry");
        auto water = std::make_unique<Checkbox>(Point(86, 45), Size(16, 16), Colour::Transparent, L"Water");
        water->set_state(true);

        highlight->on_state_changed += on_highlight;
        triggers->on_state_changed += on_show_triggers;
        hidden_geometry->on_state_changed += on_show_hidden_geometry;
        water->on_state_changed += on_show_water;

        auto enabled = std::make_unique<Checkbox>(Point(12, 70), Size(16, 16), Colour::Transparent, L"Depth");
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

        parent.add_child(std::move(rooms_groups));
    }

    void ViewOptions::set_depth(int32_t value)
    {
        _depth->set_value(value);
    }
    
    void ViewOptions::set_depth_enabled(bool value)
    {
        _enabled->set_state(value);
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
