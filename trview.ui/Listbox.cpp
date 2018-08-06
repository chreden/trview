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
            std::vector<std::pair<uint32_t, ListboxItem>> new_items;
            for(auto i = 0; i < items.size(); ++i)
            {
                new_items.emplace_back(i, items[i]);
            }

            for (const auto& item : new_items)
            {
                auto row = std::make_unique<StackPanel>(Point(), Size(), Colour(1.0f, 0.4f, 0.4f, 0.4f), Size(), Direction::Horizontal);
                for (const auto& header : _headers)
                {
                    auto button = std::make_unique<Button>(Point(), Size(30, 20), item.second.value(header));
                    row->add_child(std::move(button));
                }
                add_child(std::move(row));
            }
            _items = new_items;
        }

        void Listbox::sort_items()
        {
            if (!_current_sort.empty())
            {
                // Sort the items list by the specified key.
                std::sort(
                    _items.begin(),
                    _items.end(),
                    [&](const auto& l, const auto& r)
                {
                    return l.second.value(_current_sort) < r.second.value(_current_sort);
                });

                std::vector<std::unique_ptr<Control>> child_store;
                std::move(_child_elements.begin() + 1, _child_elements.end(), std::back_inserter(child_store));
                _child_elements.erase(_child_elements.begin() + 1, _child_elements.end());

                for(auto i = 0; i < _items.size(); ++i)
                {
                    // This is the index into the child elements list - it indicates where it used
                    // to be. We can then take the element from there and move it to the new location.
                    uint32_t old_index = _items[i].first;

                    add_child(std::move(child_store[old_index]));

                    _items[i].first = i;
                }

                on_heirarchy_changed();
            }
        }
    }
}
