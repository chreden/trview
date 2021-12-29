#include "Tooltip.h"
#include <trview.ui/Layouts/StackLayout.h>
#include <trview.ui/TextArea.h>
#include <trview.common/Windows/Shell.h>

using namespace trview::ui;

namespace trview
{
    Tooltip::Tooltip(ui::Control& control)
    {
        _container = control.add_child(std::make_unique<ui::Window>(Size(), Colour(0.5f, 0.0f, 0.0f, 0.0f)));
        auto layout = std::make_unique<StackLayout>();
        layout->set_margin(Size(5, 5));

        _container->set_layout(std::move(layout));
        _container->set_visible(false);
        _container->set_handles_input(false);
        _container->set_z(-1);

        _area = _container->add_child(std::make_unique<ui::TextArea>(Size(200, 250), Colour::Transparent, Colour::White, nullptr));
        _area->set_handles_input(false);
        _area->set_size_mode(SizeMode::Auto);
    }

    void Tooltip::set_position(const Point& position)
    {
        _container->set_position(Point(position.x + 8, position.y + 8));
    }

    void Tooltip::set_text(const std::wstring& text)
    {
        _area->set_text(text);
    }

    void Tooltip::set_text_colour(const Colour& colour)
    {
        _area->set_text_colour(colour);
    }

    void Tooltip::set_visible(bool value)
    {
        _container->set_visible(value);
    }

    bool Tooltip::visible() const
    {
        return _container->visible();
    }
}
