#include "Dropdown.h"
#include "Label.h"
#include "Button.h"
#include "Listbox.h"

namespace trview
{
    namespace ui
    {
        Dropdown::Dropdown(const Point& position, const Size& size)
            : Window(position, size, Colour(1.0f, 0.0f, 0.0f))
        {
            _button = add_child(std::make_unique<Button>(position, size, L""));
            _token_store.add(_button->on_click += [&]()
            {
                _dropdown->set_visible(!_dropdown->visible());
                set_focus_control(this);
                update_dropdown();
            });
        }

        void Dropdown::set_dropdown_scope(ui::Control* scope)
        {
            auto dropdown = std::make_unique<Listbox>(Point(), Size(size().width, size().height * _values.size()), Colour(0.4f, 0.4f, 0.4f));
            dropdown->set_z(-1);
            dropdown->set_visible(false);
            dropdown->set_columns(
                { 
                    { Listbox::Column::Type::String, L"Name", static_cast<uint32_t>(size().width) }
                });
            dropdown->set_show_headers(false);
            dropdown->set_show_scrollbar(false);
            _token_store.add(dropdown->on_item_selected += [&](const auto& item)
            {
                auto value = item.value(L"Name");
                on_value_selected(value);
                _button->set_text(value);
                _dropdown->set_visible(false);
            });
            _dropdown = scope->add_child(std::move(dropdown));
            update_dropdown();
        }

        void Dropdown::set_values(const std::vector<std::wstring>& values)
        {
            _values = values;
            update_dropdown();
        }

        void Dropdown::set_selected_value(const std::wstring& value)
        {
            _button->set_text(value);
        }

        void Dropdown::update_dropdown()
        {
            if (!_dropdown || !_dropdown->visible())
            {
                return;
            }

            // Set the position of the dropdown to be just below us.
            _dropdown->set_position(absolute_position() + Point(0, size().height));
            _dropdown->set_size(Size(size().width, size().height * _values.size()));

            std::vector<Listbox::Item> items;
            for (const auto& value : _values)
            {
                items.push_back({{{ L"Name", value }}});
            }
            _dropdown->set_items(items);
        }

        void Dropdown::clicked_off(Control* new_focus)
        {
            if (!_dropdown)
            {
                return;
            }

            Control* parent = new_focus;
            while (parent)
            {
                if (parent == this || parent == _dropdown)
                {
                    return;
                }
                parent = parent->parent();
            }

            _dropdown->set_visible(false);
        }
    }
}
