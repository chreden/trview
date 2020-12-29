#include "Listbox.h"

namespace trview
{
    namespace ui
    {
        Listbox::Item::Item(const std::unordered_map<std::wstring, std::wstring>& values)
            : Item(values, Colour::White, { 0.25f, 0.25f, 0.25f })
        {
        }

        Listbox::Item::Item(const std::unordered_map<std::wstring, std::wstring>& values, const Colour& foreground, const Colour& background)
            : _values(values), _foreground(foreground), _background(background)
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

        Colour Listbox::Item::foreground() const
        {
            return _foreground;
        }

        Colour Listbox::Item::background() const
        {
            return _background;
        }
    }
}