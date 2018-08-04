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
            _headers = headers_element.get();
            add_child(std::move(headers_element));
        }
    }
}
