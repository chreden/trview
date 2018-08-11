#include "Listbox.h"
#include "Button.h"
#include "ListboxItemPanel.h"

namespace trview
{
    namespace ui
    {
        Listbox::Listbox(const Point& position, const Size& size)
            : StackPanel(position, size, Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(), Direction::Horizontal, SizeMode::Manual)
        {
            auto panel = std::make_unique<ListboxItemPanel>(position, size);
            _item_panel = panel.get();
            add_child(std::move(panel));

            add_child(std::make_unique<Window>(Point(), Size(20, size.height), Colour(1.0f, 1.0f, 0.0f, 0.0f)));

            _token_store.add(on_size_changed += [=](const auto& new_size)
            {
                _item_panel->set_size(new_size);
            });
        }

        Listbox::~Listbox()
        {
        }

        void Listbox::set_headers(const std::vector<std::wstring>& headers)
        {
            _item_panel->set_headers(headers);
        }

        void Listbox::set_items(const std::vector<ListboxItem>& items)
        {
            _item_panel->set_items(items);
        }
    }
}
