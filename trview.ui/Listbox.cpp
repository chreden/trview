#define NOMINMAX
#include "Listbox.h"
#include "Button.h"
#include "Scrollbar.h"
#include <algorithm>

namespace trview
{
    namespace ui
    {
        Listbox::Listbox(const Size& size, const Colour& background_colour)
            : Listbox(Point(), size, background_colour)
        {
        }

        Listbox::Listbox(const Point& position, const Size& size, const Colour& background_colour)
            : StackPanel(position, size, background_colour, Size(), Direction::Vertical, SizeMode::Manual)
        {
            _token_store += on_size_changed += [=](auto)
            {
                generate_rows();
                populate_rows();
            };
        }

        Listbox::~Listbox()
        {
        }

        void Listbox::set_columns(const std::vector<Column>& columns)
        {
            _columns = columns;
            generate_ui();
        }

        void Listbox::set_enable_sorting(bool value)
        {
            _enable_sorting = value;
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
                auto rows_container = std::make_unique<StackPanel>(Size(size().width, remaining_height), background_colour(), Size(), Direction::Horizontal, SizeMode::Manual);

                auto rows_element = std::make_unique<StackPanel>(Size(size().width - scrollbar_width, remaining_height), background_colour(), Size(), Direction::Vertical, SizeMode::Manual);
                _rows_element = rows_container->add_child(std::move(rows_element));

                if (_show_scrollbar)
                {
                    auto rows_scrollbar = std::make_unique<Scrollbar>(Size(scrollbar_width, remaining_height), background_colour());
                    _token_store += rows_scrollbar->on_scroll += [&](float value)
                    {
                        scroll_to(static_cast<uint32_t>(value * _items.size()));
                    };
                    _rows_scrollbar = rows_container->add_child(std::move(rows_scrollbar));
                }

                _rows_container = add_child(std::move(rows_container));
            }

            // Add as many rows as can be seen.
            const float row_height = 20;
            const int32_t total_required_rows = std::min<int32_t>(static_cast<int32_t>(std::ceil(remaining_height / row_height)), static_cast<int32_t>(_items.size()));
            const int32_t existing_rows = _rows_element->child_elements().size();
            const int32_t remaining_rows = total_required_rows - existing_rows;

            for (auto i = 0; i < remaining_rows; ++i)
            {
                auto row = std::make_unique<Row>(background_colour(), _columns);
                _token_store += row->on_click += [this](const auto& item) { select_item(item); };
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
            for (auto r = 0u; r < rows.size(); ++r)
            {
                const auto index = r + _current_top;
                auto row = static_cast<Row*>(rows[r]);
                if (index < _items.size())
                {
                    if (row->position().y + row->size().height <= _rows_element->size().height)
                    {
                        ++_fully_visible_rows;
                    }

                    row->set_item(_items[index]);
                    row->set_visible(true);
                }
                else
                {
                    row->set_visible(false);
                    row->clear_item();
                }
            }

            if (!_items.empty() && _rows_scrollbar)
            {
                _rows_scrollbar->set_range(static_cast<float>(_current_top), static_cast<float>(_current_top + _fully_visible_rows), static_cast<float>(_items.size()));
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
            const int32_t direction = delta > 0 ? -1 : 1;

            // If we are at the bottom of the list, don't scroll down any more.
            if (direction > 0 && _current_top + _fully_visible_rows >= _items.size())
            {
                return true;
            }

            // Don't do a negative on the unsigned index.
            if (_current_top > 0 || direction == 1)
            {
                _current_top += direction;
                populate_rows();
            }

            return true;
        }

        bool Listbox::key_down(uint16_t key)
        {
            if (key == VK_DELETE)
            {
                on_delete();
                return true;
            }

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
                if (key == VK_UP && item != _items.begin())
                {
                    // Go up if possible (not already at the start of the list)
                    select_item(*--item);
                }
                else if (key == VK_DOWN && ++item != _items.end())
                {
                    // Go down if possible (not at the end of the list).
                    select_item(*item);
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

            auto headers_element = std::make_unique<StackPanel>(size(), background_colour(), Size(), Direction::Horizontal);
            for (const auto column : _columns)
            {
                auto header_element = std::make_unique<Button>(Size(static_cast<float>(column.width()), 20.0f), column.name());
                header_element->set_text_background_colour(background_colour());
                _token_store += header_element->on_click += [this, column]()
                {
                    if (!_enable_sorting)
                    {
                        return;
                    }

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
                };
                headers_element->add_child(std::move(header_element));
            }

            // Add the spacer to make the scrollbar look ok.
            headers_element->add_child(std::make_unique<Window>(Size(10, 20), background_colour()));
            _headers_element = add_child(std::move(headers_element));
        }

        void Listbox::select_item(const Item& item, bool raise_event)
        {
            on_focus_requested();
            _selected_item = item;
            scroll_to_show(item);
            if (raise_event)
            {
                on_item_selected(_selected_item.value());
            }
        }

        void Listbox::highlight_item()
        {
            const auto rows = _rows_element->child_elements();
            for (auto& row_element : rows)
            {
                auto row = static_cast<Row*>(row_element);
                row->set_highlighted(_show_highlight && row->item() == _selected_item);
            }
        }

        void Listbox::scroll_to(uint32_t item)
        {
            // If the item is a partially visible row, move the top of the list down by one (move it into view at 
            // the bottom).
            if (item == _current_top + _fully_visible_rows)
            {
                _current_top = std::clamp<int32_t>(_current_top + 1, 0, _items.size() - _fully_visible_rows);
            }
            else
            {
                // Otherwise, just set the new item to be at the top of the list.
                _current_top = std::clamp<int32_t>(item, 0, _items.size() - _fully_visible_rows);
            }
            populate_rows();
        }

        void Listbox::scroll_to_show(const Item& item)
        {
            auto iter = std::find(_items.begin(), _items.end(), item);
            if (iter == _items.end())
            {
                return;
            }

            auto index = static_cast<uint32_t>(iter - _items.begin());

            // Scroll the list so that the selected item is visible. If it is already on the 
            // same page, then no need to scroll.
            if (index >= _current_top && index < _current_top + _fully_visible_rows)
            {
                highlight_item();
                return;
            }

            scroll_to(index);
        }

        bool Listbox::set_selected_item(const Item& item)
        {
            if (std::find(_items.begin(), _items.end(), item) == _items.end())
            {
                return false;
            }
            select_item(item, false);
            return true;
        }
    }
}
