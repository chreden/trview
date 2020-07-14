#include "Toolbar.h"
#include <trview.ui/Button.h>

using namespace trview::ui;

namespace trview
{
    Toolbar::Toolbar(Control& control)
    {
        // Create a panel at the bottom of the parent control - have to add it to the control first so we can find out where to put it.
        auto toolbar = std::make_unique<StackPanel>(Size(100, 40), Colour(0.0f, 0.0f, 0.0f, 0.0f), Size(5,5), StackPanel::Direction::Horizontal);
        _toolbar = control.add_child(std::move(toolbar));

        // Put the toolbar in the middle bottom of the window.
        auto set_position = [&](const Size& size)
        {
            _toolbar->set_position(
                Point(size.width * 0.5f - _toolbar->size().width * 0.5f,
                      size.height - _toolbar->size().height));
        };

        // Resize the toolbar.
        set_position(control.size());
        _token_store += control.on_size_changed += set_position;
        _token_store += _toolbar->on_size_changed += 
            [=](const auto&) 
        {
            set_position(_toolbar->parent()->size());
        };
    }

    void Toolbar::add_tool(const std::wstring& name, const std::wstring& text)
    {
        auto tool = std::make_unique<Button>(Size(32, 32), text);
        _token_store += tool->on_click += [=]()
        {
            on_tool_clicked(name);
        };
        _toolbar->add_child(std::move(tool));
    }
}