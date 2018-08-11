#pragma once

#include <vector>
#include <string>

#include <trview.common/TokenStore.h>

#include "StackPanel.h"
#include "ListboxItem.h"

namespace trview
{
    namespace ui
    {
        class ListboxItemPanel final : public StackPanel
        {
        public:
            ListboxItemPanel(const Point& position, const Size& size);

            /// Set the headers that will be used for sorting and filtering items.
            /// @param headers The header names.
            void set_headers(const std::vector<std::wstring>& headers);

            /// Set the items for the list box.
            /// @param items The items to add to the list box.
            void set_items(const std::vector<ListboxItem>& items);
        private:
            /// Populate the row UI elements with the values from the current listbox item sort.
            void populate_rows();

            void sort_items();

            StackPanel*               _headers_element;
            std::vector<std::wstring> _headers;

            StackPanel*               _rows_element;

            std::vector<ListboxItem> _items;

            std::wstring              _current_sort;
            bool                      _current_sort_direction;
            TokenStore                _token_store;
        };
    }
}
