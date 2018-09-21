#define NOMINMAX
#include "Listbox.h"
#include "Button.h"
#include "Scrollbar.h"
#include <algorithm>

namespace trview
{
    namespace ui
    {
        Listbox::Column::Column()
            : _type(Type::String)
        {
        }

        Listbox::Column::Column(Type type, const std::wstring& name, uint32_t width)
            : _type(type), _name(name), _width(width)
        {
        }

        Listbox::Column::Type Listbox::Column::type() const
        {
            return _type;
        }

        const std::wstring& Listbox::Column::name() const
        {
            return _name;
        }

        uint32_t Listbox::Column::width() const
        {
            return _width;
        }

        Listbox::Item::Item(const std::unordered_map<std::wstring, std::wstring>& values)
            : _values(values)
        {
        }

        std::wstring Listbox::Item::value(const std::wstring& key) const
        {
            auto item = _values.find(key);
            if (item == _values.end())
            {
                return std::wstring();
            }
            return item->second;
        }

        bool Listbox::Item::operator == (const Item& other) const
        {
            return _values == other._values;
        }

        Listbox::Listbox(const Point& position, const Size& size, const Colour& background_colour)
            : StackPanel(position, size, background_colour, Size(), Direction::Vertical, SizeMode::Manual)
        {
            _token_store.add(on_size_changed += [=](auto)
            {
                generate_rows();
                populate_rows();
            });
        }

        Listbox::~Listbox()
        {
        }

        void Listbox::set_columns(const std::vector<Column>& columns)
        {
            _columns = columns;
            generate_ui();
        }

        void Listbox::set_items(const std::vector<Item>& items)
        {
            // Reset the index for scrolling.
            _current_top = 0;

            // Store the items for later.
            _items = items;

            generate_rows();
            populate_rows();

            // Scroll to the highlighted item (if present in the list).
            if (_show_highlight && _selected_item.has_value())
            {
                scroll_to_show(_selected_item.value());
            }
        }

        void Listbox::generate_rows()
        {
            // Calculate how many items can be seen on-screen at once.
            const auto header_height = _headers_element ? _headers_element->size().height : 0.0f;
            const auto remaining_height = size().height - header_height;

            // If negative height, this is a bad thing to try and set the size of the rows to, so abort.
            if (static_cast<int32_t>(remaining_height) <= 0)
            {
                return;
            }

            const auto scrollbar_width = _show_scrollbar ? 10.0f : 0.0f;

            if (_rows_container)
            {
                _rows_container->set_size(Size(size().width, remaining_height));
                _rows_element->set_size(Size(size().width - scrollbar_width, remaining_height));
                if (_rows_scrollbar)
                {
                    _rows_scrollbar->set_size(Size(scrollbar_width, remaining_height));
                }
            }
            else
            {
                auto rows_container = std::make_unique<StackPanel>(Point(), Size(size().width, remaining_height), background_colour(), Size(), Direction::Horizontal, SizeMode::Manual);

                auto rows_element = std::make_unique<StackPanel>(Point(), Size(size().width - scrollbar_width, remaining_height), background_colour(), Size(), Direction::Vertical, SizeMode::Manual);
                _rows_element = rows_container->add_child(std::move(rows_element));

                if (_show_scrollbar)
                {
                    auto rows_scrollbar = std::make_unique<Scrollbar>(Point(), Size(scrollbar_width, remaining_height), background_colour());
                    _token_store.add(rows_scrollbar->on_scroll += [&](float value)
                    {
                        scroll_to(value * _items.size());
                    });
                    _rows_scrollbar = rows_container->add_child(std::move(rows_scrollbar));
                }

                _rows_container = add_child(std::move(rows_container));
            }

            // Add as many rows as can be seen.
            const float row_height = 20;
            const int32_t total_required_rows = std::min<int32_t>(std::ceil(remaining_height / row_height), _items.size());
            const int32_t existing_rows = _rows_element->child_elements().size();
            const int32_t remaining_rows = total_required_rows - existing_rows;

            for (auto i = 0; i < remaining_rows; ++i)
            {
                auto index = i + existing_rows;

                auto row = std::make_unique<StackPanel>(Point(), Size(), background_colour(), Size(), Direction::Horizontal);
                for (const auto& column : _columns)
                {
                    auto button = std::make_unique<Button>(Point(), Size(column.width(), 20), L"Test");
                    _token_store.add(button->on_click += [this, index]()
                    {
                        select_item(_items[index + _current_top]);
                    });
                    row->add_child(std::move(button));
                }
                _rows_element->add_child(std::move(row));
            }
        }

        void Listbox::populate_rows()
        {
            if (!_rows_element)
            {
                return;
            }

            _fully_visible_rows = 0;
            const auto rows = _rows_element->child_elements();
            for (auto r = 0; r < rows.size(); ++r)
            {
                if (r + _current_top < _items.size())
                {
                    if (!rows[r]->visible())
                    {
                        rows[r]->set_visible(true);
                    }

                    if (rows[r]->position().y + rows[r]->size().height <= _rows_element->size().height)
                    {
                        ++_fully_visible_rows;
                    }

                    const auto& item = _items[r + _current_top];
                    const auto columns = rows[r]->child_elements();
                    for (auto c = 0; c < _columns.size(); ++c)
                    {
                        static_cast<Button*>(columns[c])->set_text(item.value(_columns[c].name()));
                    }
                }
                else
                {
                    rows[r]->set_visible(false);
                }
            }

            if (!_items.empty() && _rows_scrollbar)
            {
                _rows_scrollbar->set_range(_current_top, _current_top + _fully_visible_rows, _items.size());
            }

            highlight_item();
        }

        void Listbox::sort_items()
        {
            if (!_current_sort.name().empty())
            {
                // Sort the items list by the specified key.
                std::sort(_items.begin(), _items.end(),
                    [&](const auto& l, const auto& r)
                {
                    if (_current_sort.type() == Column::Type::Number)
                    {
                        if (!_current_sort_direction)
                        {
                            return std::stoi(l.value(_current_sort.name())) < std::stoi(r.value(_current_sort.name()));
                        }
                        return std::stoi(l.value(_current_sort.name())) > std::stoi(r.value(_current_sort.name()));
                    }

                    if (!_current_sort_direction)
                    {
                        return l.value(_current_sort.name()) < r.value(_current_sort.name());
                    }
                    return l.value(_current_sort.name()) > r.value(_current_sort.name());
                });
            }
            populate_rows();
        }

        bool Listbox::scroll(int delta)
        {
            int32_t direction = delta > 0 ? -1 : 1;
            if (direction > 0)
            {
                // If any of the rows are invisible (they are being hidden because we are at the end
                // of the list of items) then do not do any more scrolling down. Only elements that are
                // invisible and fully in the client area count towards this.
                const auto rows = _rows_element->child_elements();
                if (std::any_of(rows.begin(), rows.end(), [](const auto& r) { return !r->visible() && r->position().y + r->size().height <= r->parent()->size().height; }))
                {
                    return true;
                }

                // If the bottom of the list is already visible, then don't scroll down any more.
                if (!rows.empty())
                {
                    if (_current_top + _fully_visible_rows >= _items.size())
                    {
                        return true;
                    }
                }
            }

            _current_top = std::max(0, _current_top + direction);
            populate_rows();
            return true;
        }

        bool Listbox::key_down(uint16_t key)
        {
            if (key != VK_UP && key != VK_DOWN)
            {
                return false;
            }

            // Find the selected item in the list.
            auto item = std::find(_items.begin(), _items.end(), _selected_item);

            // If the item isn't in the list but there are items in the list, select the first item in the list.
            if (item == _items.end())
            {
                if (!_items.empty())
                {
                    select_item(_items.front());
                }
            }
            else
            {
                // Go up if possible (not already at the start of the list)
                if (key == VK_UP)
                {
                    if (item == _items.begin())
                    {
                        return false;
                    }
                    select_item(*--item);
                    return true;
                }

                // Go down if possible (not at the end of the list).
                if (key == VK_DOWN && ++item != _items.end())
                {
                    select_item(*item);
                    return true;
                }
            }
            return true;
        }

        void Listbox::set_show_scrollbar(bool value)
        {
            _show_scrollbar = value;
            generate_ui();
        }

        void Listbox::set_show_headers(bool value)
        {
            _show_headers = value;
            generate_ui();
        }

        void Listbox::set_show_highlight(bool value)
        {
            _show_highlight = value;
            generate_ui();
        }

        void Listbox::generate_ui()
        {
            _headers_element = nullptr;
            _rows_container = nullptr;
            _rows_element = nullptr;
            _rows_scrollbar = nullptr;

            clear_child_elements();
            generate_headers();
            generate_rows();
            populate_rows();
        }

        void Listbox::generate_headers()
        {
            if (!_show_headers)
            {
                return;
            }

            auto headers_element = std::make_unique<StackPanel>(Point(), size(), background_colour(), Size(), Direction::Horizontal);
            for (const auto column : _columns)
            {
                auto header_element = std::make_unique<Button>(Point(), Size(column.width(), 20), column.name());
                _token_store.add(header_element->on_click += [this, column]()
                {
                    if (_current_sort.name() == column.name())
                    {
                        _current_sort_direction = !_current_sort_direction;
                    }
                    else
                    {
                        _current_sort = column;
                        _current_sort_direction = false;
                    }
                    sort_items();
                });
                headers_element->add_child(std::move(header_element));
            }

            // Add the spacer to make the scrollbar look ok.
            headers_element->add_child(std::make_unique<Window>(Point(), Size(10, 20), background_colour()));
            _headers_element = add_child(std::move(headers_element));
        }

        void Listbox::select_item(const Item& item)
        {
            _selected_item = item;
            scroll_to_show(item);
            on_item_selected(_selected_item.value());
        }

        void Listbox::highlight_item()
        {
            const auto rows = _rows_element->child_elements();
            for (auto i = 0; i < rows.size(); ++i)
            {
                // Default colour - not highlighted.
                Colour colour{ 1.0f, 0.4f, 0.4f, 0.4f };

                const auto index = i + _current_top;
                if (_show_highlight && index < _items.size() && _selected_item == _items[index])
                {
                    colour = Colour(1.0f, 0.5f, 0.5f, 0.5f);
                }

                const auto columns = rows[i]->child_elements();
                for (auto& cell : columns)
                {
                    Button* button_cell = static_cast<Button*>(cell);
                    button_cell->set_text_background_colour(colour);
                }
            }
        }

        void Listbox::scroll_to(uint32_t item)
        {
            _current_top = std::clamp<int32_t>(item, 0, _items.size() - _fully_visible_rows);
            populate_rows();
        }

        void Listbox::scroll_to_show(const Item& item)
        {
            auto iter = std::find(_items.begin(), _items.end(), item);
            if (iter == _items.end())
            {
                return;
            }

            // Scroll the list so that the selected item is visible. If it is already on the 
            // same page, then no need to scroll.
            auto index = iter - _items.begin();
            if (index < _current_top)
            {
                _current_top = index;
            }
            else if (index >= _current_top + _fully_visible_rows)
            {
                _current_top = index - _fully_visible_rows + 1;
            }

            scroll_to(iter - _items.begin());
        }
    }
}
