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
        class ListboxItemPanel;

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
            ListboxItemPanel*         _item_panel;
            TokenStore                _token_store;
        };
    }
}
