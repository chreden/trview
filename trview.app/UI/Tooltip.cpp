#include "Tooltip.h"
#include <trview.ui/StackPanel.h>
#include <trview.ui/Label.h>

using namespace trview::ui;

namespace trview
{
    Tooltip::Tooltip(ui::Control& control)
    {
        auto container = std::make_unique<StackPanel>(Size(), Colour(0.5f, 0.0f, 0.0f, 0.0f));
        container->set_margin(Size(5, 5));
        container->set_visible(false);
        container->set_handles_input(false);

        auto label = std::make_unique<ui::Label>(Point(500, 0), Size(38, 30), Colour::Transparent, L"0", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre, SizeMode::Auto);
        label->set_handles_input(false);

        _label = container->add_child(std::move(label));
        _container = control.add_child(std::move(container));
    }

    void Tooltip::set_position(const Point& position)
    {
        _container->set_position(Point(position.x - _container->size().width, position.y - _container->size().height));
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
