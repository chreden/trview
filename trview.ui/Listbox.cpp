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
            for (const auto& header : headers)
            {
                auto header_element = std::make_unique<Button>(Point(), Size(30, 20), header);
                headers_element->add_child(std::move(header_element));
            }
            _headers_element = headers_element.get();
            add_child(std::move(headers_element));
            _headers = headers;
        }

        void Listbox::set_items(const std::vector<ListboxItem>& items)
        {
            for (const auto& item : items)
            {
                auto row = std::make_unique<StackPanel>(Point(), Size(), Colour(1.0f, 0.4f, 0.4f, 0.4f), Size(), Direction::Horizontal);
                for (const auto& header : _headers)
                {
                    auto button = std::make_unique<Button>(Point(), Size(30, 20), item.value(header));
                    row->add_child(std::move(button));
                }
                add_child(std::move(row));
            }
            _items = items;
        }
    }
}
