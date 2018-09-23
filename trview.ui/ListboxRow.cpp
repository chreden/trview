#include "Listbox.h"
#include "Button.h"

namespace trview
{
    namespace ui
    {
        Listbox::Row::Row(const Colour& colour, const std::vector<Listbox::Column>& columns)
            : StackPanel(Point(), Size(), colour, Size(), Direction::Horizontal), _columns(columns)
        {
            for (const auto& column : columns)
            {
                auto button = std::make_unique<Button>(Point(), Size(column.width(), 20), L" ");
                _token_store.add(button->on_click += [this]
                {
                    if (_item.has_value())
                    {
                        on_click(_item.value());
                    }
                });
                add_child(std::move(button));
            }
        }

        void Listbox::Row::set_item(const Item& item)
        {
            _item = item;

            const auto columns = child_elements();
            for (auto c = 0; c < _columns.size(); ++c)
            {
                static_cast<Button*>(columns[c])->set_text(item.value(_columns[c].name()));
            }
        }

        void Listbox::Row::clear_item()
        {
            _item.reset();
        }

        void Listbox::Row::set_row_colour(const Colour& colour)
        {
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
    }
}
