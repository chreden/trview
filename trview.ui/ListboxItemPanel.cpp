#define NOMINMAX
#include "ListboxItemPanel.h"
#include "Button.h"
#include <algorithm>

namespace trview
{
    namespace ui
    {
        ListboxItemPanel::ListboxItemPanel(const Point& position, const Size& size)
            : StackPanel(position, size, Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(), Direction::Vertical, SizeMode::Manual)
        {

        }

        void ListboxItemPanel::set_headers(const std::vector<std::wstring>& headers)
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

        void ListboxItemPanel::set_items(const std::vector<ListboxItem>& items)
        {
            // Calculate how many items can be seen on-screen at once.
            const auto remaining_height = size().height - (_headers_element->size().height);

            // Clear the rows element so new elements can be added (this to be changed at some point to create less rows,
            // when perhaps some rows can be re-used).
            if (_rows_element)
            {
                _rows_element->clear_child_elements();
            }
            else
            {
                auto rows_element = std::make_unique<StackPanel>(Point(), Size(size().width, remaining_height), Colour(1.0f, 0.4f, 0.4f, 0.4f), Size(), Direction::Vertical, SizeMode::Manual);
                _rows_element = rows_element.get();
                add_child(std::move(rows_element));
            }

            // Add as many rows as can be seen.
            const float row_height = 20;
            const auto required_rows = std::min<uint32_t>(remaining_height / row_height, items.size());

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

            // Store the items for later.
            _items = items;
            populate_rows();
        }

        void ListboxItemPanel::populate_rows()
        {
            const auto rows = _rows_element->child_elements();
            for (auto r = 0; r < rows.size(); ++r)
            {
                const auto& item = _items[r];
                const auto columns = rows[r]->child_elements();
                for (auto c = 0; c < _headers.size(); ++c)
                {
                    static_cast<Button*>(columns[c])->set_text(item.value(_headers[c]));
                }
            }
        }

        void ListboxItemPanel::sort_items()
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
    }
}
