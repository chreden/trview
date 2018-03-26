#include "stdafx.h"
#include "Neighbours.h"

#include "ITextureStorage.h"

#include <trview.ui/Control.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Button.h>
#include <trview.ui/Label.h>
#include <trview.ui/NumericUpDown.h>

namespace trview
{
    Neighbours::Neighbours(ui::Control& parent, const ITextureStorage& texture_storage)
    {
        using namespace ui;

        auto group = std::make_unique<GroupBox>(Point(), Size(140, 50), Colour(1.0f, 0.5f, 0.5f, 0.5f), Colour(1.0f, 0.0f, 0.0f, 0.0f), L"Neighbours");
        auto enabled = std::make_unique<Button>(Point(12, 20), Size(16, 16), texture_storage.lookup("check_off"), texture_storage.lookup("check_on"));
        enabled->on_click += [&](bool value) { on_enabled_changed(value); };
        _enabled = enabled.get();

        auto depth_label = std::make_unique<Label>(Point(32, 20), Size(40, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"Depth", 10.f, TextAlignment::Left, ParagraphAlignment::Centre);
        auto depth = std::make_unique<NumericUpDown>(Point(90, 16), Size(40, 20), Colour(1.0f, 0.4f, 0.4f, 0.4f), texture_storage.lookup("numeric_up"), texture_storage.lookup("numeric_down"), 0, 10);
        depth->set_value(1);
        depth->on_value_changed += [&](int value) { on_depth_changed(value); };

        group->add_child(std::move(enabled));
        group->add_child(std::move(depth_label));
        group->add_child(std::move(depth));
        parent.add_child(std::move(group));
    }

    // Set whether neighbours are enabled. This will not raise the on_enabled_changed event.
    // value: Whether neighbours are enabled.
    void Neighbours::set_enabled(bool value)
    {
        _enabled->set_state(value);
    }
}
