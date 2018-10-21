#include "Toolbar.h"

using namespace trview::ui;

namespace trview
{
    Toolbar::Toolbar(Control& control)
    {
        // Create a panel at the bottom of the parent control - have to add it to the control first so we can find out where to put it.
        auto toolbar = std::make_unique<StackPanel>(Point(), Size(100, 20), Colour(0.5f, 0.5f, 0.5f), Size(2,0), StackPanel::Direction::Horizontal);
        _toolbar = control.add_child(std::move(toolbar));

        // Put the toolbar in the middle bottom of the window.
        auto set_position = [&](const Size& size)
        {
            _toolbar->set_position(
                Point(size.width * 0.5f - _toolbar->size().width * 0.5f,
                      size.height - _toolbar->size().height));
        };

        set_position(control.size());
        _token_store.add(control.on_size_changed += set_position);
    }
}