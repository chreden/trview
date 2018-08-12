/// @file Listbox.h
/// @brief UI element that displays a number of rows in a grid with column headers.

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
        /// Displays a number of rows in a grid with column headers.
        class Listbox : public StackPanel
        {
        public:
            Listbox(const Point& position, const Size& size);

            virtual ~Listbox();

            /// Set the headers that will be used for sorting and filtering items.
            /// @param headers The header names.
            void set_headers(const std::vector<std::wstring>& headers);

            /// Set the items for the list box.
            /// @param items The items to add to the list box.
            void set_items(const std::vector<ListboxItem>& items);
        protected:
            virtual bool scroll(int delta) override;
        private:
            /// Create the row UI elements to be populated.
            void generate_rows();
            /// Populate the row UI elements with the values from the current listbox item sort.
            void populate_rows();
            /// Sort the items according to the current sort method.
            void sort_items();

            StackPanel*               _headers_element;
            std::vector<std::wstring> _headers;

            StackPanel*               _rows_element;

            std::vector<ListboxItem> _items;
            int32_t                  _current_top{ 0 };

            // Sorting options.
            std::wstring              _current_sort;
            bool                      _current_sort_direction;
            TokenStore                _token_store;
        };
    }
}
