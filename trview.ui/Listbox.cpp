#define NOMINMAX
#include "Listbox.h"
#include "Button.h"
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

        void Listbox::set_columns(const std::vector<Column>& columns)
        {
            auto headers_element = std::make_unique<StackPanel>(Point(), size(), Colour(1.0f, 0.3f, 0.3f, 0.3f), Size(), Direction::Horizontal);
            for (const auto column : columns)
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
            _headers_element = headers_element.get();
            add_child(std::move(headers_element));
            _columns = columns;
        }

        void Listbox::set_items(const std::vector<Item>& items)
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
            const auto remaining_height = size().height - _headers_element->size().height;

            // If negative height, this is a bad thing to try and set the size of the rows to, so abort.
            if (static_cast<int32_t>(remaining_height) <= 0)
            {
                return;
            }

            if (_rows_element)
            {
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
            const int32_t total_required_rows = std::min<int32_t>(std::ceil(remaining_height / row_height), _items.size());
            const int32_t existing_rows = _rows_element->child_elements().size();
            const int32_t remaining_rows = total_required_rows - existing_rows;

            for (auto i = 0; i < remaining_rows; ++i)
            {
                auto index = i + existing_rows;

                auto row = std::make_unique<StackPanel>(Point(), Size(), Colour(1.0f, 0.4f, 0.4f, 0.4f), Size(), Direction::Horizontal);
                for (const auto& column : _columns)
                {
                    auto button = std::make_unique<Button>(Point(), Size(column.width(), 20), L"Test");
                    _token_store.add(button->on_click += [this, index]()
                    {
                        on_item_selected(_items[index + _current_top]);
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
                if (std::any_of(rows.begin(), rows.end(), [](const auto& r) { return !r->visible() && r->position().y + r->size().height < r->parent()->size().height; }))
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
