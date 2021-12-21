#include "Tooltip.h"
#include <trview.ui/Layouts/StackLayout.h>
#include <trview.ui/Label.h>

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

        _label = _container->add_child(std::make_unique<ui::Label>(Point(500, 0), Size(38, 30), Colour::Transparent, L"0", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre, SizeMode::Auto));
        _label->set_handles_input(false);
    }

    void Tooltip::set_position(const Point& position)
    {
        _container->set_position(Point(position.x + 8, position.y + 8));
    }

    void Tooltip::set_text(const std::wstring& text)
    {
        _label->set_text(text);
    }

    void Tooltip::set_text_colour(const Colour& colour)
    {
        _label->set_text_colour(colour);
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
