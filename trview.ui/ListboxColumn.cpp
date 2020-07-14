#include "Listbox.h"

namespace trview
{
    namespace ui
    {
        Listbox::Column::Column()
            : _type(Type::String)
        {
        }

        Listbox::Column::Column(Type type, const std::wstring& name, uint32_t width)
            : _type(type), _name(name), _width(width)
        {
        }

        Listbox::Column::Type Listbox::Column::type() const
        {
            return _type;
        }

        const std::wstring& Listbox::Column::name() const
        {
            return _name;
        }

        uint32_t Listbox::Column::width() const
        {
            return _width;
        }
    }
}
