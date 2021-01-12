#include "Listbox.h"
#include "Button.h"
#include "Checkbox.h"

namespace trview
{
    namespace ui
    {
        const std::string Listbox::Row::Names::cell_name_format{ "cell-" };

        Listbox::Row::Row(const Colour& colour, const std::vector<Listbox::Column>& columns)
            : StackPanel(Point(), Size(), colour, Size(), Direction::Horizontal), _columns(columns)
        {
            set_handles_hover(true);

            for (const auto& column : columns)
            {
                switch (column.type())
                {
                    case Column::Type::Boolean:
                    {
                        auto panel = add_child(std::make_unique<Button>(Size(static_cast<float>(column.width()), 20.0f), L" "));
                        auto checkbox = panel->add_child(std::make_unique<Checkbox>());
                        checkbox->set_name(Listbox::Row::Names::cell_name_format + to_utf8(column.name()));

                        auto size = checkbox->size();
                        checkbox->set_position(Point(static_cast<float>(static_cast<int>(column.width() / 2.0f - size.width / 2.0f)), 2.0f));

                        auto name = column.name();
                        _token_store += checkbox->on_state_changed += [this, name](bool value)
                        {
                            if (_item.has_value())
                            {
                                on_state_changed(_item.value(), name, value);
                            }
                        };
                        break;
                    }
                    default:
                    {
                        auto button = add_child(std::make_unique<Button>(Size(static_cast<float>(column.width()), 20.0f), L" "));
                        button->set_name(Listbox::Row::Names::cell_name_format + to_utf8(column.name()));
                        _token_store += button->on_click += [this]
                        {
                            if (_item.has_value())
                            {
                                on_click(_item.value());
                            }
                        };
                        break;
                    }
                }
            }
        }

        void Listbox::Row::set_item(const Item& item)
        {
            _item = item;
            set_background_colour(item.background());

            const auto columns = child_elements();
            for (auto c = 0u; c < _columns.size(); ++c)
            {
                switch (_columns[c].type())
                {
                    case Column::Type::Boolean:
                    {
                        Checkbox* checkbox = columns[c]->find<Checkbox>(Listbox::Row::Names::cell_name_format + to_utf8(_columns[c].name()));
                        checkbox->set_state(std::stoi(item.value(_columns[c].name())) == 1);
                        break;
                    }
                    default:
                    {
                        Button* button = static_cast<Button*>(columns[c]);
                        button->set_text(item.value(_columns[c].name()));
                        button->set_text_background_colour(item.background());
                        button->set_text_colour(item.foreground());
                        break;
                    }
                }
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
                Button* button_cell = dynamic_cast<Button*>(cell);
                if (button_cell)
                {
                    button_cell->set_text_background_colour(colour);
                }
                else
                {
                    Window* checkbox_cell = dynamic_cast<Window*>(cell);
                    if (checkbox_cell)
                    {
                        checkbox_cell->set_background_colour(colour);
                    }
                }
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
