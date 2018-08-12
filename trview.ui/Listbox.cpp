#define NOMINMAX
#include "Listbox.h"
#include "Button.h"
#include <algorithm>

namespace trview
{
    namespace ui
    {
        Listbox::Listbox(const Point& position, const Size& size)
            : StackPanel(position, size, Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(), Direction::Vertical, SizeMode::Manual)
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

        void Listbox::set_headers(const std::vector<std::wstring>& headers)
        {
            auto headers_element = std::make_unique<StackPanel>(Point(), size(), Colour(1.0f, 0.3f, 0.3f, 0.3f), Size(), Direction::Horizontal);
            for (const auto header : headers)
            {
                auto header_element = std::make_unique<Button>(Point(), Size(30, 20), header);
                _token_store.add(header_element->on_click += [this, header]()
                {
                    if (_current_sort == header)
                    {
                        _current_sort_direction = !_current_sort_direction;
                    }
                    else
                    {
                        _current_sort = header;
                        _current_sort_direction = false;
                    }
                    sort_items();
                });
                headers_element->add_child(std::move(header_element));
            }
            _headers_element = headers_element.get();
            add_child(std::move(headers_element));
            _headers = headers;
        }

        void Listbox::set_items(const std::vector<ListboxItem>& items)
        {
            // Reset the index for scrolling.
            _current_top = 0;

            // Store the items for later.
            _items = items;

            generate_rows();
            populate_rows();
        }

        void Listbox::generate_rows()
        {
            // Calculate how many items can be seen on-screen at once.
            const auto remaining_height = size().height - (_headers_element->size().height);

            // If negative height, this is a bad thing to try and set the size of the rows to, so abort.
            if (static_cast<int32_t>(remaining_height) <= 0)
            {
                return;
            }

            // Clear the rows element so new elements can be added.
            if (_rows_element)
            {
                _rows_element->clear_child_elements();
                _rows_element->set_size(Size(size().width, remaining_height));
            }
            else
            {
                auto rows_element = std::make_unique<StackPanel>(Point(), Size(size().width, remaining_height), Colour(1.0f, 0.4f, 0.4f, 0.4f), Size(), Direction::Vertical, SizeMode::Manual);
                _rows_element = rows_element.get();
                add_child(std::move(rows_element));
            }

            // Add as many rows as can be seen.
            const float row_height = 20;
            const auto required_rows = std::min<uint32_t>(std::ceil(remaining_height / row_height), _items.size());

            for (auto i = 0; i < required_rows; ++i)
            {
                auto row = std::make_unique<StackPanel>(Point(), Size(), Colour(1.0f, 0.4f, 0.4f, 0.4f), Size(), Direction::Horizontal);
                for (const auto& header : _headers)
                {
                    auto button = std::make_unique<Button>(Point(), Size(30, 20), L"Test");
                    row->add_child(std::move(button));
                }
                _rows_element->add_child(std::move(row));
            }
        }

        void Listbox::populate_rows()
        {
            const auto rows = _rows_element->child_elements();
            for (auto r = 0; r < rows.size(); ++r)
            {
                if (r + _current_top < _items.size())
                {
                    if (!rows[r]->visible())
                    {
                        rows[r]->set_visible(true);
                    }

                    const auto& item = _items[r + _current_top];
                    const auto columns = rows[r]->child_elements();
                    for (auto c = 0; c < _headers.size(); ++c)
                    {
                        static_cast<Button*>(columns[c])->set_text(item.value(_headers[c]));
                    }
                }
                else
                {
                    rows[r]->set_visible(false);
                }
            }
        }

        void Listbox::sort_items()
        {
            if (!_current_sort.empty())
            {
                // Sort the items list by the specified key.
                std::sort(_items.begin(), _items.end(),
                    [&](const auto& l, const auto& r)
                {
                    if (!_current_sort_direction)
                    {
                        return std::stoi(l.value(_current_sort)) < std::stoi(r.value(_current_sort));
                    }
                    return std::stoi(l.value(_current_sort)) > std::stoi(r.value(_current_sort));
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
                // of the list of items) then do not do any more scrolling down.
                const auto rows = _rows_element->child_elements();
                if (std::any_of(rows.begin(), rows.end(), [](const auto& r) { return !r->visible(); }))
                {
                    return true;
                }

                // If the bottom of the list is already visible, then don't scroll down any more.
                if (!rows.empty())
                {
                    const auto& last = rows.back();
                    if (last->position().y + last->size().height < _rows_element->size().height)
                    {
                        return true;
                    }
                }
            }

            _current_top = std::max(0, _current_top + direction);
            populate_rows();
            return true;
        }
    }
}
