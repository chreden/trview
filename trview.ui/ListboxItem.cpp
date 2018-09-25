#include "Listbox.h"

namespace trview
{
    namespace ui
    {
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

        bool Listbox::Item::operator == (const Item& other) const
        {
            return _values == other._values;
        }
    }
}