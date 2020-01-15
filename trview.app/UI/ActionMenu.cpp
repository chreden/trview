#include "ActionMenu.h"

using namespace trview::ui;

namespace trview
{
    ActionMenu::ActionMenu(Control& parent)
    {
        _test_button = parent.add_child(std::make_unique<Button>(Size(64, 64), L"Sprint"));
        _test_button->set_visible(false);
    }

    void ActionMenu::show(const Point& position)
    {
        _test_button->set_visible(true);
        _test_button->set_position(position);
    }
}
