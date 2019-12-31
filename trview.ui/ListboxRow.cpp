#include "Listbox.h"
#include "Button.h"

namespace trview
{
    namespace ui
    {
        Listbox::Row::Row(const Colour& colour, const std::vector<Listbox::Column>& columns)
            : StackPanel(Point(), Size(), colour, Size(), Direction::Horizontal), _columns(columns)
        {
            set_handles_hover(true);

            for (const auto& column : columns)
            {
                auto button = std::make_unique<Button>(Size(static_cast<float>(column.width()), 20.0f), L" ");
                _token_store += button->on_click += [this]
                {
                    if (_item.has_value())
                    {
                        on_click(_item.value());
                    }
                };
                add_child(std::move(button));
            }
        }

        void Listbox::Row::set_item(const Item& item)
        {
            _item = item;
            set_background_colour(item.background());

            const auto columns = child_elements();
            for (auto c = 0u; c < _columns.size(); ++c)
            {
                Button* button = static_cast<Button*>(columns[c]);
                button->set_text(item.value(_columns[c].name()));
                button->set_text_background_colour(item.background());
                button->set_text_colour(item.foreground());
            }
        }

        void Listbox::Row::clear_item()
        {
            _item.reset();
        }

        void Listbox::Row::update_row_colour()
        {
            Colour colour = background_colour();
            if (_highlighted || _hovered)
            {
                colour += Colour(0.0f, 0.1f, 0.1f, 0.1f);
            }

            const auto columns = child_elements();
            for (auto& cell : columns)
            {
                Button* button_cell = static_cast<Button*>(cell);
                button_cell->set_text_background_colour(colour);
            }
        }

        const std::optional<Listbox::Item>& Listbox::Row::item() const
        {
            return _item;
        }

        void Listbox::Row::mouse_enter()
        {
            _hovered = true;
            update_row_colour();
        }

        void Listbox::Row::mouse_leave()
        {
            _hovered = false;
            update_row_colour();
        }

        void Listbox::Row::set_highlighted(bool value)
        {
            _highlighted = value;
            update_row_colour();
        }
    }
}
