#include "ContextMenu.h"

namespace trview
{
    ContextMenu::ContextMenu(ui::Control& parent)
    {
        _menu = parent.add_child(std::make_unique<ui::Window>(Point(300, 300), Size(200, 100), Colour(1.0f, 0.0f, 0.0f)));
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
