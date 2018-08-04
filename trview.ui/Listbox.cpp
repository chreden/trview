#include "Listbox.h"
#include "Button.h"

namespace trview
{
    namespace ui
    {
        Listbox::Listbox(const Point& position, const Size& size)
            : StackPanel(position, size, Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(), Direction::Vertical, SizeMode::Manual)
        {
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
                header_element->on_click += [this, header]()
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
                    set_items(_items);
                };
                headers_element->add_child(std::move(header_element));
            }
            _headers_element = headers_element.get();
            add_child(std::move(headers_element));
            _headers = headers;
        }

        void Listbox::set_items(const std::vector<ListboxItem>& items)
        {
            // Sort the items.
            std::vector<ListboxItem> sorted_items = items;
            if (!_current_sort.empty())
            {
                std::sort(
                    sorted_items.begin(),
                    sorted_items.end(),
                    [&](const auto& l, const auto& r)
                {
                    return l.value(_current_sort) < r.value(_current_sort);
                });
            }

            for (const auto& item : sorted_items)
            {
                auto row = std::make_unique<StackPanel>(Point(), Size(), Colour(1.0f, 0.4f, 0.4f, 0.4f), Size(), Direction::Horizontal);
                for (const auto& header : _headers)
                {
                    auto button = std::make_unique<Button>(Point(), Size(30, 20), item.value(header));
                    row->add_child(std::move(button));
                }
                add_child(std::move(row));
            }
            _items = sorted_items;
        }
    }
}
