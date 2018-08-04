#include "ListboxItem.h"

namespace trview
{
    namespace ui
    {
        ListboxItem::ListboxItem(const std::unordered_map<std::wstring, std::wstring>& values)
            : _values(values)
        {
        }

        std::wstring ListboxItem::value(const std::wstring& key) const
        {
            auto item = _values.find(key);
            if (item == _values.end())
            {
                return std::wstring();
            }
            return item->second;
        }
    }
}
