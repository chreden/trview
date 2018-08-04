#pragma once


#include <vector>
#include <string>

#include "StackPanel.h"

namespace trview
{
    namespace ui
    {
        class Listbox : public StackPanel
        {
        public:
            Listbox(const Point& position, const Size& size);

            virtual ~Listbox();

            /// Set the headers that will be used for sorting and filtering items.
            /// @param headers The header names.
            void set_headers(const std::vector<std::wstring>& headers);
        private:
            StackPanel * _headers;
        };
    }
}
