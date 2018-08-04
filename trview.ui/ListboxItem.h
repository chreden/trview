#pragma once

#include <unordered_map>

namespace trview
{
    namespace ui
    {
        class ListboxItem final
        {
        public:
            ListboxItem(const std::unordered_map<std::wstring, std::wstring>& values);

            /// Get the value that the item has for the specified key.
            /// @param key The key to search for.
            /// @returns The value for the key.
            std::wstring value(const std::wstring& key) const;
        private:
            std::unordered_map<std::wstring, std::wstring> _values;
        };
    }
}
