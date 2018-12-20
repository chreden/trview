#include "ContextMenu.h"
#include <trview.ui/Button.h>

using namespace trview::ui;

namespace trview
{
    ContextMenu::ContextMenu(Control& parent)
    {
        _menu = parent.add_child(std::make_unique<StackPanel>(Point(300, 300), Size(200, 100), Colour(0.2f, 0.2f, 0.2f), Size(), StackPanel::Direction::Horizontal));

        auto button = std::make_unique<Button>(Point(), Size(24, 24), L"+");
        button->on_click += on_add_waypoint;
        _menu->add_child(std::move(button));

        _menu->set_visible(false);
    }

    void ContextMenu::set_position(const Point& position)
    {
        _menu->set_position(position);
    }

    void ContextMenu::set_visible(bool value)
    {
        _menu->set_visible(value);
    }
}
