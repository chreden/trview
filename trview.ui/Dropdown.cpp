#include "Dropdown.h"
#include "Label.h"
#include "Button.h"
#include "Listbox.h"

namespace trview
{
    namespace ui
    {
        Dropdown::Dropdown(const Point& position, const Size& size)
            : Window(position, size, Colour::Transparent)
        {
            _button = add_child(std::make_unique<Button>(position, size, L""));
            _button->set_background_colour(Colour(0.25f, 0.25f, 0.25f));
            _token_store += _button->on_click += [&]()
            {
                if (!_dropdown)
                {
                    return;
                }
                _dropdown->set_visible(!_dropdown->visible());
                on_focus_requested();
                update_dropdown();
            };
        }

        Dropdown::Dropdown(const Size& size)
            : Dropdown(Point(), size)
        {
        }

        void Dropdown::set_dropdown_scope(ui::Control* scope)
        {
            auto dropdown = std::make_unique<Listbox>(Point(), Size(size().width, size().height * _values.size()), Colour(0.25f, 0.25f, 0.25f));
            dropdown->set_z(-1);
            dropdown->set_visible(false);
            dropdown->set_columns(
                { 
                    { Listbox::Column::Type::String, L"Name", static_cast<uint32_t>(size().width) }
                });
            dropdown->set_show_headers(false);
            dropdown->set_show_scrollbar(false);
            _token_store += dropdown->on_item_selected += [&](const auto& item)
            {
                auto value = item.value(L"Name");
                on_value_selected(value);
                _button->set_text(value);
                _dropdown->set_visible(false);
            };
            _dropdown = scope->add_child(std::move(dropdown));
            update_dropdown();
        }

        void Dropdown::set_values(const std::vector<std::wstring>& value_names)
        {
            std::vector<Value> values;
            std::transform(value_names.begin(), value_names.end(), std::back_inserter(values), 
                [](const std::wstring& name) -> Value
            {
                return { name, { 0.25f, 0.25f, 0.25f }, Colour::White };
            });
            set_values(values);
        }

        void Dropdown::set_values(const std::vector<Value>& values)
        {
            _values = values;
            update_dropdown();
        }

        void Dropdown::set_selected_value(const std::wstring& value)
        {
            _button->set_text(value);
        }

        void Dropdown::set_text_colour(const Colour& colour)
        {
            _button->set_text_colour(colour);
        }

        void Dropdown::set_text_background_colour(const Colour& colour)
        {
            _button->set_text_background_colour(colour);
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
                items.push_back({{{ L"Name", value.text }}, value.foreground, value.background});
            }
            _dropdown->set_items(items);
        }

        void Dropdown::lost_focus(Control* new_focus)
        {
            Window::lost_focus(new_focus);

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
