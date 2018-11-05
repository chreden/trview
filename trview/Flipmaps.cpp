#include "stdafx.h"
#include "Flipmaps.h"
#include <trview.ui/GroupBox.h>
#include <trview.ui/Checkbox.h>

namespace trview
{
    Flipmaps::Flipmaps(ui::Control& parent)
    {
        using namespace ui;
        auto flips_group = std::make_unique<GroupBox>(Point(), Size(140, 145), Colour(1.0f, 0.5f, 0.5f, 0.5f), Colour(1.0f, 0.0f, 0.0f, 0.0f), L"Flipmaps");

        // Add two methods of controlling flipmaps:
        // The TR1-3 method:
        auto tr1_3_panel = std::make_unique<Window>(Point(10, 15), Size(120, 120), Colour(1.0f, 1.0f, 0.0f, 0.0f));
        auto flip = std::make_unique<Checkbox>(Point(12, 42), Size(16, 16), L"Flip");
        flip->on_state_changed += on_flip;
        _flip = tr1_3_panel->add_child(std::move(flip));
        _tr1_3_panel = flips_group->add_child(std::move(tr1_3_panel));

        // The TR4-5 method:
        auto tr4_5_panel = std::make_unique<Window>(Point(10, 15), Size(120, 120), Colour(1.0f, 0.0f, 1.0f, 0.0f));
        tr4_5_panel->set_visible(false);
        _tr4_5_panel = flips_group->add_child(std::move(tr4_5_panel));

        parent.add_child(std::move(flips_group));
    }

    void Flipmaps::set_alternate_groups(bool value)
    {
        _tr1_3_panel->set_visible(!value);
        _tr4_5_panel->set_visible(value);
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

