#include "stdafx.h"
#include "Neighbours.h"

#include <trview.app/ITextureStorage.h>

#include <trview.ui/Control.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Label.h>
#include <trview.ui/NumericUpDown.h>

namespace trview
{
    Neighbours::Neighbours(ui::Control& parent, const ITextureStorage& texture_storage)
    {
        using namespace ui;

        auto group = std::make_unique<GroupBox>(Point(), Size(150, 50), Colour::Transparent, Colour::Grey, L"Neighbours");
        auto enabled = std::make_unique<Checkbox>(Point(12, 20), Size(16, 16), Colour::Transparent, L"Depth");
        enabled->on_state_changed += on_enabled_changed;

        auto depth = std::make_unique<NumericUpDown>(Point(90, 16), Size(40, 20), Colour::Transparent, texture_storage.lookup("numeric_up"), texture_storage.lookup("numeric_down"), 0, 10);
        depth->set_value(1);
        depth->on_value_changed += on_depth_changed;

        _enabled = group->add_child(std::move(enabled));
        _depth = group->add_child(std::move(depth));
        parent.add_child(std::move(group));
    }

    // Set whether neighbours are enabled. This will not raise the on_enabled_changed event.
    // value: Whether neighbours are enabled.
    void Neighbours::set_enabled(bool value)
    {
        _enabled->set_state(value);
    }

    void Neighbours::set_depth(int32_t value)
    {
        _depth->set_value(value);
    }
}
